
 //
 //		main loop
 //

#include "mc_shared_mem.h"
#include "scope_init.h"
#include "scope_main.h"

#define HTRIGGER_POS_LEFT_OF_DISPLAY	2	// 水平トリガーは画面の外左側
#define HTRIGGER_POS_RIGHT_OF_DISPLAY	1	// 水平トリガーは画面の外右側
#define HTRIGGER_POS_MIDDLE_OF_DISPLAY	0	//　水平トリガーは画面の中

#define CHECK_STATUS_UPDATE if(MCV->UpdateFlags > 0)goto loop_end
int scope_main(void){

	//scope_init();

    // ループ制御用変数
	uint16_t i = 0;
	uint8_t htrigger_pos_stat = HTRIGGER_POS_MIDDLE_OF_DISPLAY;	// トリガーの位置
	uint32_t cont=0; 	// DMA Control　毎回書き直すから必要
	uint32_t lli=0,dest_addr=0;
	int16_t h_trigger_left_side = (MCV->SampleLength/2) + MCV->HTrigger;	// 画面の左端を0とした場合のh_trigger
	uint32_t trigger_edge_reg_mask = 1<<3;	//up cross


    while(1) {

    	//
    	//		main loop start
    	//

    	while(MCV->RunningMode==RUNMODE_STOP);
    	CHECK_STATUS_UPDATE;

    	start_adc();

    	while(MCV->RoleModeOn){	//フリーモード低速時ロールビューモード
    		CHECK_STATUS_UPDATE;
    		LPC_GPDMA->C0LLI	 = lli_role_view.NextLLI;	// LLIが書き換わらないことがあるので応急処置
    	}

    	// フリーモードか水平トリガーは画面の外左側でないなら
    	if((htrigger_pos_stat != HTRIGGER_POS_LEFT_OF_DISPLAY) || (MCV->RunningMode == RUNMODE_FREE) || (MCV->Timescale <= 5)) {	// 80Mspsの時はサンプル飛び対策のため必ず待つ
    		// バッファがたまるのを待つ
    		//2ndLLIが読み込まれるまで待機		★時間のかかる処理
    		while(!(LPC_GPDMA->INTTCSTAT&0x1))CHECK_STATUS_UPDATE;		// DMA Ch0のターミナルカウント割り込みフラグ監視
    	}

    	CHECK_STATUS_UPDATE;

    	//
    	// ここからタイミングがシビア
    	//

    	if(MCV->RunningMode == RUNMODE_NORMAL){

    		// トリガー割り込みフラグはなかなか消えない
    		// トリガー割り込みフラグが消えるまでクリアし続ける
			do{
				LPC_ADCHS->INTS[1].CLR_STAT = 3<<2;
			} while(LPC_ADCHS->INTS[1].STATUS & trigger_edge_reg_mask);

			while(!(LPC_ADCHS->INTS[1].STATUS & trigger_edge_reg_mask));	// waiting trigger	★時間のかかる処理
			CHECK_STATUS_UPDATE;

			//if(htrigger_pos_stat != HTRIGGER_POS_RIGHT_OF_DISPLAY) {	// 水平トリガーは画面の外右側でないなら
				LPC_TIMER1->TCR = 1;							// Timer Enable
				while(!(LPC_TIMER1->IR&0x1)){
					CHECK_STATUS_UPDATE;					// waiting timer match	★時間のかかる処理
				}
			//}

			stop_adc();

			//
			// ここまでタイミングがシビア
			//

			CHECK_STATUS_UPDATE;

			//
			// 水平トリガー位置探索開始アドレス、停止アドレス設定
			// i = 水平トリガー位置探索開始アドレス
			// stop_i = 水平トリガー位置探索停止アドレス

			uint16_t stop_i;	// 探索停止位置
			uint16_t dma_stop_i = BUFFER_SIZE_HW - ((LPC_GPDMA->C0CONTROL & 0xfff)<<1);		// DMA転送が停止したアドレス

			// second_lliが読み込まれている時
			// サンプル開始indexが0な時
			if(LPC_GPDMA->C0LLI == adc_second_lli.NextLLI){
				if(htrigger_pos_stat == HTRIGGER_POS_LEFT_OF_DISPLAY){	// トリガーが画面外左側で一回目
					i = 0;//-(MCV->HTrigger + MCV->SampleLength/2);
				} else {
					i = h_trigger_left_side;	// トリガー位置までに最低限必要なサンプル量
				}

			} else {	// third_lliか4th_lliが読み込まれている時		= head:dma_stop+1  tall:dma_stop
						// またはトリガーが画面外左側にあるとき
				i = dma_stop_i+1;
				if(htrigger_pos_stat != HTRIGGER_POS_LEFT_OF_DISPLAY) {
					i = i + h_trigger_left_side;
				}

			}

			i &= BUFFER_COUNTER_MASK;
			stop_i = (dma_stop_i - (MCV->SampleLength-h_trigger_left_side)) & BUFFER_COUNTER_MASK;


			// トリガー探索
			Bool lower;	// buffer[i-1]の値がトリガー値より低い場合TRUE
			lower = (MCV->TriggerEdge==TRIGGER_EDGE_UP)? FALSE : TRUE ;

			while(1){
				if(buffer.mem_HW[i] >= MCV->VTrigger){	// トリガーを上回る
					if((lower==TRUE) && (MCV->TriggerEdge==TRIGGER_EDGE_UP)){		// アップクロス検出
						break;
					}
					lower = FALSE;
				} else{
					if((lower==FALSE) && (MCV->TriggerEdge==TRIGGER_EDGE_DOWN)){	// ダウンクロス検出
						break;
					}
					lower = TRUE;
				}
				i = (i+1)&BUFFER_COUNTER_MASK;

				if(i==stop_i){	//	トリガーが見つからない
					break;
				}
			}

			// 非連続領域が表示されないかチェック
//			if((dma_stop_i - i)<((MCV->SampleLength/2)-(MCV->HTrigger))){
//				goto loop_end;
//			}

			// ノイズリジェクション
//			if(buffer.mem_HW[(i-8)&BUFFER_COUNTER_MASK] > buffer.mem_HW[(i+8)&BUFFER_COUNTER_MASK] ){
//				goto loop_end;
//			}

			// バッファ空きを待つ
			while(MCV->BufferIsPending == TRUE);

			// 非連続領域が無いようにメモリーを整形する
			uint16_t j = i-((MCV->SampleLength/2) + MCV->HTrigger);			//i = トリガー位置　j = サンプルの先端
			// 送信バッファにコピー
			for(i=0;i<MCV->SampleLength;i++){
				j = j & BUFFER_COUNTER_MASK;
				MCV->Buffer[i] = buffer.mem_HW[j];		//memory copy
				j++;
			}

			CHECK_STATUS_UPDATE;

		//
		//		FREE RUN mode
		//
    	} else if (MCV->RunningMode == RUNMODE_FREE){

    		stop_adc();

			// バッファ空きを待つ
			while(MCV->BufferIsPending == TRUE);

			// ステータス変更があったら
			if(MCV->UpdateFlags > 0)goto loop_end;

			// 送信バッファにコピー
			for(i=0;i<MCV->SampleLength;i++){
				MCV->Buffer[i] = buffer.mem_HW[i];		//memory copy
			}

    	}

    	MCV->BufferIsPending = TRUE;

loop_end:
		stop_adc();


		h_trigger_left_side = ((MCV->SampleLength/2) + MCV->HTrigger);

    	// ステータスアップデート
    	if(MCV->UpdateFlags > 0){
			if(MCV->RunningMode == RUNMODE_NORMAL){
				int16_t transfer_size_word = h_trigger_left_side / 2;
				if(transfer_size_word > 0 ){
					cont	= DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,transfer_size_word);
					lli		= adc_first_lli.NextLLI;
					adc_second_lli.DstAddr = adc_first_lli.DstAddr + transfer_size_word * 4;
					adc_second_lli.Control = DMA_SET_TRANSFER_SIZE(adc_second_lli.Control,BUFFER_SIZE_WORD-transfer_size_word);

					if(transfer_size_word >= (MCV->SampleLength/2)){
						htrigger_pos_stat = HTRIGGER_POS_RIGHT_OF_DISPLAY;
					} else {
						htrigger_pos_stat = HTRIGGER_POS_MIDDLE_OF_DISPLAY;
					}

				} else {	// トリガーが画面外の左側
					cont	= adc_third_lli.Control;
					lli 	= adc_second_lli.NextLLI;		// 3rd lliへ（一回目判別用）
					htrigger_pos_stat = HTRIGGER_POS_LEFT_OF_DISPLAY;
				}

				if(MCV->Timescale <=5 ){	// 80MSpsの時にLLIの切り替え地点でサンプル飛びが発生するので応急処置
					cont	= DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,BUFFER_SIZE_WORD);
					lli		= adc_second_lli.NextLLI;
				}

				dest_addr = adc_first_lli.DstAddr;

				if(MCV->TriggerEdge == TRIGGER_EDGE_UP){
					trigger_edge_reg_mask = 1<<3;	// up cross
				} else {
					trigger_edge_reg_mask = 1<<2;	// down cross
				}

			} else if (MCV->RunningMode == RUNMODE_FREE){

				if(MCV->RoleModeOn){
					dest_addr = lli_role_view.DstAddr;
					lli = lli_role_view.NextLLI;
				} else {
					dest_addr = adc_first_lli.DstAddr;
					lli = 0;	// 次のLLIを読み込まない
				}

				cont	= DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,(MCV->SampleLength/2));

			}

			MCV->UpdateFlags = 0;

    	} else if(MCV->OneShotMode == TRUE){		// SINGLEモードでトリガーした後の処理

			MCV->RunningMode = RUNMODE_STOP;		// STOPにする

		}

    	// 毎回書き直す
		LPC_GPDMA->C0CONTROL = cont;
		LPC_GPDMA->C0DESTADDR = dest_addr;
		LPC_GPDMA->C0LLI	 = lli;
    	LPC_GPDMA->INTTCCLEAR = 0x1<<DMACH_ADCHS;		// DMAターミナルカウント割り込みフラグクリア
    	LPC_TIMER1->TC = 0;								// Timer counter reset
    	LPC_TIMER1->IR = 0x1;						// Timer割り込みフラグ消去
    }
    return 0 ;
}

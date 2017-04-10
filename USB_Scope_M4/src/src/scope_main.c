
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

    // ループ制御用変数
	uint16_t i 							= 0;
	uint8_t htrigger_pos_stat 			= HTRIGGER_POS_MIDDLE_OF_DISPLAY;	// トリガーの位置
	uint32_t cont=0,lli=0,dest_addr		= 0; 	// DMA Control　毎回書き直すから必要
	int16_t h_trigger_left_side 		= (MCV->SampleLength/2) + MCV->HTrigger;	// 画面の左端を0とした場合のh_trigger
	uint32_t trigger_edge_reg_mask 		= 1<<3;	//up cross
	Bool lli_update_flag 				= FALSE;					// auto free モード用 Normalに切り替えるときに使う CHECK_STATUS_UPDATEに引っかからない


    while(1) {

    	//
    	//		main loop start
    	//

    	while(MCV->RunningMode==RUNMODE_STOP){
    		__WFI();
    	}
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

    	//
    	// ここからタイミングがシビア
    	//

    	if(MCV->RunningMode == RUNMODE_NORMAL || MCV->RunningMode == RUNMODE_AUTO_FREE || MCV->RunningMode == RUNMODE_AUTO_NORMAL){

    		// トリガー割り込みフラグはなかなか消えない
    		// トリガー割り込みフラグが消えるまでクリアし続ける
			do{
				LPC_ADCHS->INTS[1].CLR_STAT = 3<<2;
			} while(LPC_ADCHS->INTS[1].STATUS & trigger_edge_reg_mask);


			if(MCV->RunningMode == RUNMODE_AUTO_FREE){	// AUTO FREE

				while(1){
					// waiting trigger	★時間のかかる処理
					if(LPC_ADCHS->INTS[1].STATUS & trigger_edge_reg_mask){  // トリガーしたかどうか
						MCV->RunningMode = RUNMODE_AUTO_NORMAL;				// AUTO_NORMALモードに移行
						lli_update_flag = TRUE;								// LLI update
						AUTO_MODE_TIMER->TC = 0;							// Timer count reset
						AUTO_MODE_TIMER->TCR = 1;							// Timer count start
						break;
					}

					if((LPC_GPDMA->C0LLI != (uint32_t)&auto_free_lli) && (MCV->BufferIsPending == FALSE)){		// 一画面分のサンプルが溜まったている状態でバッファが開きなら

						if(MCV->Timescale <= TIMESCALE_1US){		// 80Mspsの時はADCを停めないとコピー中にバッファが書き換えられる
							stop_adc();
						}

						uint16_t idx = (LPC_GPDMA->C0DESTADDR & BUFFER_ADRESS_MASK) / sizeof(uint16_t);		// index抽出
						idx = (idx - MCV_BUFFER_SAMPLE_LENGTH) & BUFFER_COUNTER_MASK;						// copy start address

						// adc buffer copy to tx buffer
						for( i=0 ; i<MCV_BUFFER_SAMPLE_LENGTH ; i++ ){
							MCV->Buffer[i] = buffer.mem_HW[(idx & BUFFER_COUNTER_MASK)];
							idx++;
						}

						MCV->BufferIsPending = TRUE;

						if(MCV->Timescale <= TIMESCALE_1US){
							LPC_GPDMA->C0CONTROL 	= cont;
							LPC_GPDMA->C0DESTADDR 	= dest_addr;
							LPC_GPDMA->C0LLI	 	= lli;
							start_adc();
						}
					}

					CHECK_STATUS_UPDATE;
				}

			} else {	// NORMAL or AUTO_NORMAL
				// waiting trigger	★時間のかかる処理
				while(!(LPC_ADCHS->INTS[1].STATUS & trigger_edge_reg_mask)){
					CHECK_STATUS_UPDATE;
				}
			}

			if(MCV->RunningMode == RUNMODE_AUTO_NORMAL) {		// auto free timer reset
				AUTO_MODE_TIMER->TC = 0;
			}

			//if(htrigger_pos_stat != HTRIGGER_POS_RIGHT_OF_DISPLAY) {	// 水平トリガーは画面の外右側でないなら
				ADC_STOP_TIMER->TCR = 1;							// Timer Enable
				while(!(ADC_STOP_TIMER->IR&0x1)){
					CHECK_STATUS_UPDATE;					// waiting timer match	★時間のかかる処理
				}
			//}

			stop_adc();

			//
			// ここまでタイミングがシビア
			//

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
					if((lower==TRUE) && (MCV->TriggerEdge==TRIGGER_EDGE_UP)){		// up cross detected
						break;
					}
					lower = FALSE;
				} else{
					if((lower==FALSE) && (MCV->TriggerEdge==TRIGGER_EDGE_DOWN)){	// down cross detected
						break;
					}
					lower = TRUE;
				}
				i = (i+1)&BUFFER_COUNTER_MASK;

				if(i==stop_i){	//	トリガーが見つからない
					break;
				}
			}

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
    	if( MCV->UpdateFlags || lli_update_flag){

    		//
    		//	DMA Register rewrite
    		//

			int16_t trgRequiredLength_wd = h_trigger_left_side / 2;	// トリガー開始位置まで最低限必要なサンプルサイズ 32bitワード
			uint16_t sampleLength_wd = MCV->SampleLength / 2;				// 32bit ワード換算の画面に表示されるサンプル長

			if(MCV->RunningMode == RUNMODE_NORMAL || MCV->RunningMode == RUNMODE_AUTO_FREE || MCV->RunningMode == RUNMODE_AUTO_NORMAL){
				dest_addr = (uint32_t)buffer.mem_W;		// バッファの先頭

				if(MCV->RunningMode == RUNMODE_NORMAL|| MCV->RunningMode == RUNMODE_AUTO_NORMAL) {	// NORMAL MODE or AUTO_NORMAL
					if(trgRequiredLength_wd > 0 ){	// 水平トリガーが画面内、もしくは画面外右側
						cont	= DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,trgRequiredLength_wd);
						lli		= adc_first_lli.NextLLI;
						adc_second_lli.DstAddr = adc_first_lli.DstAddr + trgRequiredLength_wd * 4;
						adc_second_lli.Control = DMA_SET_TRANSFER_SIZE(adc_second_lli.Control,BUFFER_SIZE_WORD-trgRequiredLength_wd);

						if(trgRequiredLength_wd >= sampleLength_wd){
							htrigger_pos_stat = HTRIGGER_POS_RIGHT_OF_DISPLAY;		// 水平トリガーが画面外右側
						} else {
							htrigger_pos_stat = HTRIGGER_POS_MIDDLE_OF_DISPLAY;		// 水平トリガーが画面内
						}

					} else {	// 水平トリガーが画面外の左側
						cont	= adc_third_lli.Control;
						lli 	= adc_second_lli.NextLLI;		// 3rd lliへ（一回目判別用）
						htrigger_pos_stat = HTRIGGER_POS_LEFT_OF_DISPLAY;
					}

					if(MCV->Timescale <=5 ){	// 80MSpsの時にLLIの切り替え地点でサンプル飛びが発生するので応急処置
						cont	= DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,BUFFER_SIZE_WORD);
						lli		= adc_second_lli.NextLLI;
					}
				} else {
					// AUTO_FREE
					/* auto freeモードメモ
					 *
					 * 画面表示に必要な長さのサンプルが溜まったら送信する、その間にトリガーがかかったらNormalモードに移行する
					 *
					 */

					lli		= (uint32_t) &auto_free_lli;

					if(trgRequiredLength_wd > 0 ){	// 水平トリガーが画面内、もしくは画面外右側

						// lli reg -> auto_free_lli -> 2nd_lli -> 3rd_lli

						auto_free_lli.NextLLI = (uint32_t) &adc_second_lli;

						if(trgRequiredLength_wd >= sampleLength_wd){			// 水平トリガーが画面外右側
							cont	=  DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,sampleLength_wd );
							auto_free_lli.DstAddr = adc_first_lli.DstAddr + sizeof(MCV->Buffer);
							auto_free_lli.Control = DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,trgRequiredLength_wd - sampleLength_wd);

							adc_second_lli.DstAddr = adc_first_lli.DstAddr + trgRequiredLength_wd * sizeof(uint32_t);
							adc_second_lli.Control = DMA_SET_TRANSFER_SIZE(adc_second_lli.Control,BUFFER_SIZE_WORD-trgRequiredLength_wd);

							htrigger_pos_stat = HTRIGGER_POS_RIGHT_OF_DISPLAY;

						} else {													// 水平トリガーが画面内
							cont	= DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,trgRequiredLength_wd);
							auto_free_lli.DstAddr = adc_first_lli.DstAddr + trgRequiredLength_wd * sizeof(uint32_t);
							auto_free_lli.Control = DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,sampleLength_wd - trgRequiredLength_wd);

							adc_second_lli.DstAddr = adc_first_lli.DstAddr + sizeof(MCV->Buffer);
							adc_second_lli.Control = DMA_SET_TRANSFER_SIZE(adc_second_lli.Control,BUFFER_SIZE_WORD-sampleLength_wd);

							htrigger_pos_stat = HTRIGGER_POS_MIDDLE_OF_DISPLAY;
						}

					} else {	// 水平トリガーが画面外の左側

						// lli reg -> auto_free_lli -> 3rd_lli
						cont	= DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,sampleLength_wd );

						auto_free_lli.DstAddr = adc_first_lli.DstAddr + sizeof(MCV->Buffer);
						auto_free_lli.Control = DMA_SET_TRANSFER_SIZE(adc_second_lli.Control,BUFFER_SIZE_WORD-sampleLength_wd);
						auto_free_lli.NextLLI = (uint32_t) &adc_third_lli;		// -> 3rd LLI

						htrigger_pos_stat = HTRIGGER_POS_LEFT_OF_DISPLAY;
					}

					if(MCV->Timescale <=5 ){	// 80MSpsの時にLLIの切り替え地点でサンプル飛びが発生するので応急処置
						cont	= DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,BUFFER_SIZE_WORD);
						lli		= adc_second_lli.NextLLI;
					}

				}


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

				cont	= DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,sampleLength_wd);

			}

			MCV->UpdateFlags = 0;
			lli_update_flag = FALSE;

    	} else if(MCV->OneShotMode == TRUE){		// SINGLEモードでトリガーした後の処理

			MCV->RunningMode = RUNMODE_STOP;		// STOPにする

		}

    	// 毎回書き直す
		LPC_GPDMA->C0CONTROL 	= cont;
		LPC_GPDMA->C0DESTADDR 	= dest_addr;
		LPC_GPDMA->C0LLI	 	= lli;
    	LPC_GPDMA->INTTCCLEAR 	= 0x1<<DMACH_ADCHS;		// DMAターミナルカウント割り込みフラグクリア

    	ADC_STOP_TIMER->TC 			= 0;					// Timer counter reset
    	ADC_STOP_TIMER->IR 			= 0x1;					// Timer割り込みフラグ消去
    }
    return 0 ;
}

// Timer2 handler
// トリガーが一定時間かからなかったら呼ばれる
 void auto_timer_handler(){

	 AUTO_MODE_TIMER->IR 			= 0x1;					// Timer割り込みフラグ消去
	 AUTO_MODE_TIMER->TC 			= 0;					// Timer counter reset

	 if(MCV->RunningMode == RUNMODE_AUTO_NORMAL){
		 MCV->RunningMode = RUNMODE_AUTO_FREE;					// AUTO_NORMAL -> AUTO_FREE
		 MCV->UpdateFlags = 1;
	 }
}


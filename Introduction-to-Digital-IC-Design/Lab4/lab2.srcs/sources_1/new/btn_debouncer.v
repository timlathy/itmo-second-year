module btn_debouncer(
    input clk,
    input btn,
    output reg btn_state
);
    reg [2:0] btn_cnt;
    reg btn_sync_0, btn_sync_1;
    
    wire cnt_reached_max = &btn_cnt;
    
    always @(posedge clk) begin
        btn_sync_0 <= btn;
        btn_sync_1 <= btn_sync_0;
        
        if (btn_state == btn_sync_1)
            btn_cnt <= 0;
        else begin
            btn_cnt <= btn_cnt + 1; 
            if (cnt_reached_max)
                btn_state <= ~btn_state; 
        end
     end
endmodule
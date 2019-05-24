`timescale 1ns / 1ps

`timescale 1ns / 1ps
 
module crc8(
    input clk,
    input rst,
    input start,
    input [7:0] val,
    output reg [7:0] data,
    output reg busy
);
    parameter InitData = 8'b11111111; // any non-zero value
 
    reg [2:0] iter;
   
    wire input_value;
    assign input_value  = val[iter];
    wire result_in_0, result_in_4, result_in_6, result_in_7;
    assign result_in_7 = (data[7] ^ input_value) ^ data[6];
    assign result_in_6 = (data[7] ^ input_value) ^ data[4];
    assign result_in_4 = (data[7] ^ input_value) ^ data[0];
    assign result_in_0 = data[7] ^ input_value;
 
    always@(posedge clk)
        if (rst)
            data <= InitData;
        else
            if (busy) begin
                iter <= iter + 1;
                data <= {result_in_7, result_in_6,
                    data[4], result_in_4,
                    data[2], data[1],
                    data[0], result_in_0};
                if (iter == 7)
                    busy <= 0;
            end
        else
            if (start) begin
                iter <= 0;
                busy <= 1;
            end
endmodule

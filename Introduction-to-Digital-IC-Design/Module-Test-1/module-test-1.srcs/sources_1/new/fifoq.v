`timescale 1ns / 1ps

module fifoq(
    input rst,
    input clk,
    input [7:0] din, // data
    input wr,
    input rd,
    output [7:0] dout
);
    reg [7:0] queue [3:0]; // four 8-bit values
    reg [2:0] curr_item;
    
    assign dout = queue[0];

    always@(posedge clk)
        if (rst)
            curr_item = 3'b000;
        else begin
            if (rd || (wr && curr_item == 3'b100)) begin
                queue[0] = queue[1];
                queue[1] = queue[2];
                queue[2] = queue[3];
                curr_item = curr_item - 1;
            end
            if (wr) begin
                queue[curr_item] = din;
                curr_item = curr_item + 1;
            end
        end
endmodule
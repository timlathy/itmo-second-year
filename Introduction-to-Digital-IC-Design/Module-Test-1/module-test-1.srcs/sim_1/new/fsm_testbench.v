`timescale 1ns / 1ps

module fsm_testbench;
    reg clk_in, rst_in;
    reg [1:0] a_in;
    wire [3:0] y_out;
    
    fsm fsmtest(.rst(rst_in), .clk(clk_in), .a(a_in), .y(y_out));
    
    initial begin
        clk_in = 1'b0; // initial clock
        rst_in = 1'b1;
        @(negedge clk_in)
            rst_in = 1'b0;
            a_in = 2'b00;
        @(negedge clk_in)
            $display("S0, a = %b, output: %b", a_in, y_out);
            a_in = 2'b10;
        @(negedge clk_in)
            $display("S0, a = %b, output: %b", a_in, y_out);
        @(negedge clk_in)
            $display("S1 a = %b, output: %b", a_in, y_out);;
            a_in = 2'b01;
        @(negedge clk_in)
            $display("S1, a = %b, output: %b", a_in, y_out);
            a_in = 2'b10;
        @(negedge clk_in)
            $display("S0, a = %b, output: %b", a_in, y_out);
            a_in = 2'b00;
        @(negedge clk_in)
            $display("S1, a = %b, output: %b", a_in, y_out);
        @(negedge clk_in)      
            $display("S2, a = %b, output: %b", a_in, y_out);
            a_in = 2'b11;
        @(negedge clk_in) 
            $display("S2, a = %b, output: %b", a_in, y_out);   
            a_in = 2'b01;    
        @(negedge clk_in)
            $display("S3, a = %b, output: %b", a_in, y_out);
            a_in = 2'b11;
        @(negedge clk_in)      
            $display("S3, a = %b, output: %b", a_in, y_out);
            a_in = 2'b00;
        @(negedge clk_in)
            $display("S0, a = %b, output: %b", a_in, y_out);
        $stop;
    end

    always
        #20 clk_in = ~clk_in; // alternate clock every 20ns (posedge every 40ns)
endmodule

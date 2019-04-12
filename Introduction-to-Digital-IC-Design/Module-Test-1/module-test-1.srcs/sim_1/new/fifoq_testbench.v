`timescale 1ns / 1ps

module fifoq_testbench;
    reg clk_in, rst_in, write_in, read_in;
    reg [0:7] data_in;
    wire [0:7] data_out;
    
    fifoq queue(.rst(rst_in), .clk(clk_in), .din(data_in), .wr(write_in), .rd(read_in), .dout(data_out));
    
    initial begin
        clk_in = 1'b0; // initial clock
        rst_in = 1'b1;
        @(negedge clk_in)
            rst_in = 1'b0;
            read_in = 1'b0;
            write_in = 1'b1;
            data_in = 3; 
        @(negedge clk_in)
            $display("|3|||| wr, dout = ", data_out);
            write_in = 1'b0;
        @(negedge clk_in)
            $display("|3||||, dout = ", data_out);
        @(negedge clk_in)
            write_in = 1'b1;
            data_in = 5;
        @(negedge clk_in)
            $display("|3|5||| wr, dout = ", data_out);
            write_in = 1'b0;
        @(negedge clk_in)
            data_in = 7;
            write_in = 1'b1;
        @(negedge clk_in)
            $display("|3|5|7|| wr, dout = ", data_out);
            write_in = 1'b0;
        @(negedge clk_in)
            data_in = 9;
            write_in = 1'b1;
        @(negedge clk_in)
            $display("|3|5|7|9| wr, dout = ", data_out);
            write_in = 1'b0;
        @(negedge clk_in)
            data_in = 11;
            write_in = 1'b1;
        @(negedge clk_in)
            $display("|5|7|9|11| wr, dout = ", data_out);
            write_in = 1'b0;
        @(negedge clk_in)
            data_in = 13;
            write_in = 1'b1;
        @(negedge clk_in)
            $display("|7|9|11|13| wr, dout = ", data_out);
            read_in = 1'b1;
        @(negedge clk_in)
            $display("|9|11|13|| wr, dout = ", data_out);
        @(negedge clk_in)
            $display("|11|13||| wr, dout = ", data_out);
        @(negedge clk_in)
            $display("|13|||| wr, dout = ", data_out);
        $stop;
    end

    always
        #20 clk_in = ~clk_in; // alternate clock every 20ns (posedge every 40ns)
endmodule

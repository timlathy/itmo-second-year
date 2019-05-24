`timescale 1ns / 1ps

module hypotenuse_testbench;
    parameter CLOCK_HALF_PERIOD = 10; // 10ns = 100MHz

    reg clk_in, rst_in;
    reg [15:0] sw_in;
    wire [15:0] hyp_out;
    wire busy_out;
    
    hypotenuse hyp(
        .clk(clk_in), .rst(rst_in), .sw(sw_in),
        .busy(busy_out), .leds(hyp_out)
    );

    initial begin
        clk_in = 0;
        forever #CLOCK_HALF_PERIOD clk_in = !clk_in;
    end
    
    integer test_i, test_expected, cycles_taken;
    
    localparam TestCount = 13;
    
    integer a_test [0:(TestCount - 1)];
    integer b_test [0:(TestCount - 1)];
    integer out_test [0:(TestCount - 1)];
    
    initial begin
        a_test[0] = 4; b_test[0] = 7; out_test[0] = 8;
        
        a_test[1] = 0; b_test[1] = 1; out_test[1] = 1;
        a_test[2] = 1; b_test[2] = 0; out_test[2] = 1;
        a_test[3] = 1; b_test[3] = 1; out_test[3] = 1;
        a_test[4] = 2; b_test[4] = 2; out_test[4] = 2;
        a_test[5] = 4; b_test[5] = 7; out_test[5] = 8;
        a_test[6] = 11; b_test[6] = 3; out_test[6] = 11;
        a_test[7] = 44; b_test[7] = 24; out_test[7] = 50;
        a_test[8] = 123; b_test[8] = 79; out_test[8] = 146;
        a_test[9] = 140; b_test[9] = 180; out_test[9] = 228;
        a_test[10] = 181; b_test[10] = 181; out_test[10] = 255;
        
        a_test[11] = 182; b_test[11] = 182; out_test[11] = 26;
        a_test[12] = 200; b_test[12] = 200; out_test[12] = 120;
        
        for (test_i = 0; test_i < TestCount; test_i = test_i + 1) begin
            rst_in = 1;
            
            repeat(2) @(posedge clk_in);
            
            rst_in = 0;
            
            sw_in[7:0] = a_test[test_i];
            sw_in[15:8] = b_test[test_i];
            test_expected = out_test[test_i];
    
            repeat(2) @(posedge clk_in);
            
            cycles_taken = 0;
            while (busy_out) begin
                repeat (1) @(posedge clk_in);
                cycles_taken <= cycles_taken + 1;
            end
            
            if (hyp_out == test_expected)
                $display("[+] sqrt(%d^2 + %d^2) = %d, cycles taken: %d", a_test[test_i], b_test[test_i], hyp_out, cycles_taken);
            else begin
                $display("Expected %d for sqrt(%d^2 + %d^2), got %d", test_expected, a_test[test_i], b_test[test_i], hyp_out);
                $stop;
            end
        end
        $finish;
    end
endmodule

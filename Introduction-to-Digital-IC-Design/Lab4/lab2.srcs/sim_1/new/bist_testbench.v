`timescale 1ns / 1ps

module bist_testbench;
    parameter CLOCK_HALF_PERIOD = 10; // 10ns = 100MHz

    reg clk_in, rst_in, test_in;
    reg [15:0] switches;
    wire [15:0] leds;
    wire test_running;
    
    bist bist(
        .clk(clk_in), .rst(rst_in), .toggle_test(test_in),
        .switches(switches), .test_running(test_running), .leds(leds)
    );

    initial begin
        clk_in = 0;
        forever #CLOCK_HALF_PERIOD clk_in = !clk_in;
    end
    
    integer test_a, test_b, test_out;
    
    initial begin
        test_a = 4; test_b = 7; test_out = 8;

        rst_in = 1;
        test_in = 0;
        
        repeat(2) @(posedge clk_in);
        
        rst_in = 0;
        
        switches[7:0] = test_a;
        switches[15:8] = test_b;

        repeat(26) @(posedge clk_in);
        
        if (leds == test_out)
            $display("[+]");
        else
            $stop;
        
        test_in = 1;
        
        repeat (2) @(posedge clk_in);
        
        if (test_running != 1)
            $stop;

        @(negedge test_running) begin
            repeat (1) @(posedge clk_in);
            if (leds[7:0] != 1)
                $stop;
        end
            
        $finish;
    end
endmodule

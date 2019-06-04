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
    
    integer test_a [0:1];
    integer test_b [0:1];
    integer test_out [0:1];
    
    initial begin
        test_a[0] = 4; test_b[0] = 7; test_out[0] = 8;
        test_a[1] = 5; test_b[1] = 12; test_out[1] = 13;

        rst_in = 1;
        test_in = 0;
        
        repeat(2) @(posedge clk_in);
        
        rst_in = 0;
        
        switches[7:0] = test_a[0];
        switches[15:8] = test_b[0];

        repeat(26) @(posedge clk_in);
        if (leds != test_out[0])
            $stop;
        
        test_in = 1;
        repeat (12) @(posedge clk_in); // debouncer delay
        if (test_running != 1)
            $stop;
        
        repeat (1) @(posedge leds[0])
        
        if (leds[15:8] != 8'b11100100)
            $stop;
        
        test_in = 0;
        repeat (12) @(posedge clk_in);
        test_in = 1;
        repeat (12) @(posedge clk_in);
        test_in = 0;
        repeat (12) @(posedge clk_in);
        
        if (test_running != 0)
            $stop;
        
        $finish;
    end
endmodule

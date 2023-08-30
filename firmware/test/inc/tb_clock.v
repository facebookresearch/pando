parameter CLOCK_PERIOD = 10.0;

task wait_cycles;
    input [31:0] cycles_to_wait;
    begin
        repeat (cycles_to_wait)
        begin
            #(CLOCK_PERIOD);
        end
    end
endtask

always
begin
    #(CLOCK_PERIOD / 2) clk <= !clk;
end

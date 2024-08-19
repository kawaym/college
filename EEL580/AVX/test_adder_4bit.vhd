LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY test_adder_4bit IS
END test_adder_4bit;

ARCHITECTURE test OF test_adder_4bit IS

	SIGNAL clock: STD_LOGIC := '0';
	SIGNAL reset: STD_LOGIC := '1';

	SIGNAL in_adder_4bit : STD_LOGIC_VECTOR(7 DOWNTO 0) := (OTHERS => '0');
	ALIAS in_a IS in_adder_4bit(3 DOWNTO 0);
	ALIAS in_b IS in_adder_4bit(7 DOWNTO 4);
	SIGNAL in_c: STD_LOGIC;

	SIGNAL out_c: STD_LOGIC;
	SIGNAL out_s: STD_LOGIC_VECTOR(3 DOWNTO 0);

BEGIN
	
	clock <= NOT clock AFTER 1 ns;
	reset <= '1', '0' AFTER 5 ns;
	
	dut: ENTITY work.adder_4bit PORT MAP (a_in => in_a, b_in => in_b, c_in => in_c, c_out => out_c, s_out => out_s);

	 stimulus:
		PROCESS BEGIN
			WAIT UNTIL (reset = '0');
			in_adder_4bit <= "00000000";
			WAIT FOR 2 ns;
			in_adder_4bit <= "00010001";
			WAIT FOR 2 ns;
			in_adder_4bit <= "00010011";
			WAIT;
		END PROCESS stimulus;
END ARCHITECTURE test;
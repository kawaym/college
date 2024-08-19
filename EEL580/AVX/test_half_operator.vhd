LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY test_half_operator IS
END test_half_operator;

ARCHITECTURE test OF test_half_operator IS

	SIGNAL clock: STD_LOGIC := '0';
	SIGNAL reset: STD_LOGIC := '1';

	SIGNAL in_half_operator : STD_LOGIC_VECTOR(1 DOWNTO 0) := (OTHERS => '0');
	ALIAS in_a IS in_half_operator(0);
	ALIAS in_b IS in_half_operator(1);
	
	SIGNAL in_mode: STD_LOGIC;

	SIGNAL out_c: STD_LOGIC;
	SIGNAL out_s: STD_LOGIC;

BEGIN
	
	clock <= NOT clock AFTER 1 ns;
	reset <= '1', '0' AFTER 5 ns;
	
	dut: ENTITY work.half_operator PORT MAP (a_in => in_a, b_in => in_b, c_out => out_c, s_out => out_s, mode_in => in_mode);

	 stimulus:
		PROCESS BEGIN
			WAIT UNTIL (reset = '0');
			in_half_operator <= "00";
			WAIT FOR 2 ns;
			in_half_operator <= "01";
			WAIT FOR 2 ns;
			in_half_operator <= "10";
			WAIT FOR 2 ns;
			in_half_operator <= "11";
			WAIT;
		END PROCESS stimulus;
END ARCHITECTURE test;
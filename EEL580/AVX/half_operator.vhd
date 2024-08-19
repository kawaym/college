LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY half_operator IS
  PORT (
  	a_in, b_in: IN STD_LOGIC;
	mode_in: IN STD_LOGIC;
  	s_out, c_out: OUT STD_LOGIC
  );
END half_operator;

ARCHITECTURE TypeArchitecture OF half_operator IS	

BEGIN

	s_out <= a_in XOR b_in;
	c_out <= (((NOT mode_in) AND (NOT a_in)) OR (mode_in AND a_in)) AND b_in;
	-- c_out <= a_in AND b_in;
	
END TypeArchitecture;
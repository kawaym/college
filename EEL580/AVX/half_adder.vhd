LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY half_adder IS
  PORT (
  	a_in, b_in: IN STD_LOGIC;
  	s_out, c_out: OUT STD_LOGIC
  );
END half_adder;

ARCHITECTURE TypeArchitecture OF half_adder IS	

BEGIN

	s_out <= a_in XOR b_in;
	c_out <= a_in AND b_in;
	
END TypeArchitecture;
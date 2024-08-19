LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY AVX IS
  PORT (
    a_in, b_in: IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    c_in: IN STD_LOGIC;
    c_out: OUT STD_LOGIC;                    
    s_out: OUT STD_LOGIC_VECTOR(3 DOWNTO 0)  
    );
END AVX;
ARCHITECTURE TypeArchitecture OF AVX IS

	SIGNAL P, Q, R: STD_LOGIC_VECTOR(3 DOWNTO 0);

	SIGNAL C1, C2, C3: STD_LOGIC;

	COMPONENT half_adder is
		PORT(a_in, b_in: IN STD_LOGIC;
  			s_out, c_out: OUT STD_LOGIC);
  	END COMPONENT;

BEGIN


	S0: half_adder PORT MAP(a_in(0),b_in(0),P(0),Q(0));
	S1: half_adder PORT MAP(a_in(1),b_in(1),P(1),Q(1));

	S2: half_adder PORT MAP(a_in(2),b_in(2),P(2),Q(2));
	S3: half_adder PORT MAP(a_in(3),b_in(3),P(3),Q(3));

	C1 <= Q(0) OR (P(0) AND c_in);
	C2 <= Q(1) OR (P(1) AND Q(0)) OR (P(1) AND P(0) AND c_in);
	C3 <= Q(2) OR (P(2) AND Q(1)) OR (P(2) AND P(1) AND Q(0)) OR (P(2) AND P(1) AND P(0) AND c_in);
	
	c_out <= Q(3) OR (P(3) AND Q(2)) OR (P(3) AND P(2) AND Q(1)) OR (P(3) AND P(2) AND P(1) AND Q(0)) OR (P(3) AND P(2) AND P(1) AND P(0) AND c_in);

	s_out(0) <= P(0) XOR c_in;
	s_out(1) <= P(1) XOR C1;
	s_out(2) <= P(2) XOR C2;
	s_out(3) <= P(3) XOR C3;

END TypeArchitecture;
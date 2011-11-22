--
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;

entity adder4 is
    port (
        A: in STD_LOGIC;
        B: in STD_LOGIC;
        Ci: in STD_LOGIC;
        S: out STD_LOGIC;
        Co: out STD_LOGIC
    );
end adder4;

architecture func of adder4 is
	component maj3 is port (
		A: in STD_LOGIC;
		B: in STD_LOGIC;
		C: in STD_LOGIC;
		Z: out STD_LOGIC);
	end component;

	signal not_Ci, Co_aux, not_Co_aux, maj_aux : STD_LOGIC;
begin

	not_Ci <= not Ci;

	MAJ_1 : maj3 port map( A, B, Ci, Co_aux );
	not_Co_aux <= not Co_aux;
	Co <= Co_aux;

	MAJ_2 : maj3  port map( A, B, not_Ci, maj_aux );

	MAJ_3 : maj3  port map( not_Co_aux, maj_aux, Ci, S );

end func;

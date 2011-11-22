--
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity adder is
    port (
        A: in STD_LOGIC;
        B: in STD_LOGIC;
        Ci: in STD_LOGIC;
        S: out STD_LOGIC;
	Co: out STD_LOGIC
    );
end adder;

architecture func of adder is
begin

	S <= A xor B xor Ci;
	Co <= (A and B) or (B and Ci) or (A and Ci);

end func;

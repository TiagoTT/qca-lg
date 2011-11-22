--
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;

entity adder2 is
    port (
        A: in STD_LOGIC_VECTOR(1 downto 0);
        B: in STD_LOGIC_VECTOR(1 downto 0);
        Ci: in STD_LOGIC;
        S: out STD_LOGIC_VECTOR(1 downto 0)
    );
end adder2;

architecture func of adder2 is
begin

--	S <= ("0"&A) + ("0"&B) + ("00"&Ci);
	S <= A + B + Ci;

end func;

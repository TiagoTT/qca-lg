--
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;

entity adder3 is
    port (
        A: in STD_LOGIC;
        B: in STD_LOGIC;
        Ci: in STD_LOGIC;
        S: out STD_LOGIC_VECTOR(1 downto 0)
    );
end adder3;

architecture func of adder3 is
begin

	S <= ("0"&A) + ("0"&B) + ("0"&Ci);

end func;

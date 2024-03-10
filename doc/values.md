# Values

## Light bulb values

| lights                                                       | **$U_f [V]$** | **$I_f [mA]$** | **$R [Ω]$** | **$R_{std} [Ω]$** | **$P [W]$** |
|--------------------------------------------------------------|:-------------:|:--------------:|:-----------:|:-----------------:|:-----------:|
| [**main beam**](./datasheets/5mm-warm-white-OSMMD251A5A.pdf) | 2.9           | 20             | 70          | 75                | ¼           |
| [**high beam**](./datasheets/3mm-cool-white-OSW5DK6C21A.pdf) | 2.9           | 20             | 70          | 75                | ¼           |
| [**tail**](./datasheets/5mm-red-OSR5PA5TK4A.pdf)             | 2.1           | 20             | 150         | 160               | ¼           |
| [**turn signal**](./datasheets/3mm-orange-OSO5PA3NE4A.pdf)   | 2.1           | 20             | 150         | 160               | ¼           |
| [**break**](./datasheets/3mm-red-OSR5PA3NE4A.pdf)            | 2.1           | 20             | 150         | 160               | ¼           |
| [**reversing**](./datasheets/3mm-cool-white-OSW5DK6CF2A.pdf) | 2.9           | 20             | 70          | 75                | ¼           |
| [**rear fog**](./datasheets/3mm-red-OSR5RU6C11A.pdf)         | 2.1           | 20             | 150         | 160               | ¼           |

### Resistance calculation

$$R = \frac{U-(U_{f}*LED_{count})}{I_{f}}$$

- $U$ is the voltage of the battery (`7.2V` in this case)
- $U_{f}$ is the forward voltage of the LED
- $I_{f}$ is the forward current of the LED.
- $R$ is the resistance of the resistor needed to connect the LED to the circuit
- $R_{std}$ is the standard resistance value of the resistor needed to connect the LED to the circuit
- $P$ is the minimal power resistance of the resistor needed to connect the LED to the circuit
- $LED_{count}$ is the number of LEDs in the circuit connected in series (`2` in this case)

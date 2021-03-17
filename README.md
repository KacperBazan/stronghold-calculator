<div align="center">
  <h1>stronghold-calculator</h1>
	<blockquote>
		<p><i>How far is this shit?</i></p>
		<p><b>In a Minecraft speedrun, the runner usually has enough obsidian blocks at the end of their bartering session to build a secondary portal to exit the overworld. Where to build the portal though? That's the question. This C++ program generates a text file with distances from spawn, and the respective average distance to the nearest stronghold. Additionally, if you want a specific distance the stronghold to be within, there is a highroll calculator that determines the best placement for a highroll portal. Top runners don't care about minimizing 1000 block blind travels, they care about maximizing the chances of super close portals. In <a href="https://www.youtube.com/watch?v=12efBB7SW9c&t=61s">this</a> video you can see an explanation of nether exiting strategy within a standard Minecraft speedrun. </b></p>
		<img src=https://img.shields.io/badge/Build-Working-brightgreen> <img src=https://img.shields.io/badge/License-MIT-yellow>
		<img src=https://img.shields.io/badge/Progress-Complete-brightgreen>
	</blockquote>
</div>

## Results
<img src="https://github.com/KacperBazan/stronghold-calculator/blob/main/img/Average%20Distance%20To%20Stronghold.png" height="550">
<img src="https://github.com/KacperBazan/stronghold-calculator/blob/main/img/Best%20Blind%20Odds%20(20%20Block).png" height="550">
<img src="https://github.com/KacperBazan/stronghold-calculator/blob/main/img/Best%20Blind%20Odds%20(300%20Block).png" height="550">

> To minimize your average distance to the stronghold, the optimal blind travel is around **~212 blocks** from the origin in the nether. In practice, anything around 200 blocks will provide sufficient results.
> To increase your chances of a highroll portal, specifically a 20 block blind travel, one would need to blind travel around **~176 blocks** or around **~203 blocks** from the origin in the nether. Since the chances are incredibly low here anyway, anything close in these ranges will provide sufficient results.

## Credits
- **SnoopTurducken#4054** - User on discord that created similar graphs in JavaScript and taught me how strongholds actually generate. Specifically he let me know about the reduced ranges of stronghold generation. He also inspired me to write my first C++ program.

For info on stronghold generation, the wiki is linked [here](https://minecraft.gamepedia.com/Stronghold)

## Installation and How To
1. If you want to download the entire repository, click on the green box and download the zip file.
2. Unzip the zip file.
3. Locate the `src` folder.
4. Locate the `stronghold_calc.cpp` and the `stronghold_highroll_calc` files.
5. Open the files in your respective IDE.
6. If you want to output the data into a specific file, change the `.txt` file name in the `writeToFile()` method in `main()`.
7. Otherwise, build and run the executable. You may change the following variables to refine your search: `STARTDIST`, `MAXDIST`, `NUM_SH`, `RES_WIDTH`, `RES_THETA`, and `THRESHOLD`.
8. Results should be printed in the desired `.txt` file and can be output into any data handling program.

## Licensing
*Copyright © 2020 Kacper Bazan* - Email: **bazankacper25@gmail.com**

*Distributed under the [MIT](https://github.com/KacperBazan/stronghold-calculator/blob/main/LICENSE) license*

## To Do's
1. Improve search algorithm to only check for strongholds within the ring of the searching radius.
2. Improve accuracy to allow increasingly small percentages. For now it would default to 0% instead of 0.04%. Mitigated by counting number of highrolls and post processing later rather than calculating actual percentages.
3. Change rings to nether coordinates. This is effectively diving the data by 8.
~~4. Add user interface to allow file naming, and the changing of the following variables: `STARTDIST`, `MAXDIST`, `NUM_SH`, `RES_WIDTH`, `RES_THETA`, and `THRESHOLD`.~~
5. Output into a `.csv` file instead of `.txt` to easily import into something like Microsoft Excel for graphing. Or maybe even create my own graphing utility who knows.

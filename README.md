
# Angaarash

>*(Old Persian)* **mathematics**

<sub><sup> Angaarash is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Angaarash is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Angaarash. If not, see <https://www.gnu.org/licenses/>.

Angaarash is an ongoing attempt to explore mathematics from an algorithmic perspective. It generally falls under the definition of a Computer Algebra System (***CAS***). New features are under active development.
___
### Features
Existing:
- Basic arithmetic
- Order of operations
- Predefined variables/functions (pi, e, trigonometric functions)
- "let" command to define custom variables/functions

Upcoming:
- Vectors
- Dot and cross products
- Multi-variable functions
- Add logarithms to predefined functions
- Matrices
- Gauss-Jordan elimination
- Solving systems of linear equations
- Complex numbers
- Arbitrary precision numbers (GMP)
- Graphing
- Solving quadratics and other polynomials
- Calculus
___
### Installation
On Windows:
1. Download the latest release package from [GitHub](https://github.com/BarZamSr/Angaarash/releases)
2. Extract the contents of the zip file
3. Open Angaarash from inside the `bin` directory

On GNU/Linux:
1. Open a terminal window and clone Angaarash with `git clone --recursive https://github.com/BarZamSr/Angaarash.git`
2. Navigate to the root of the repository with `cd Angaarash`
3. Execute the build script with an install directory of your choosing as the argument (e.g. `./scripts/build.sh ~/Desktop/Angaarash`
4. Open Angaarash from inside the `bin` directory where you chose to install (e.g. `~/Desktop/Angaarash/bin`)

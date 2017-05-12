#include "colors.inc"
#include "woods.inc"
#include "pascal.inc"
#include "puzzle.inc"

camera 
{
    up y
    right x*110/70
    location < 0, 4.9, 0 >
//    location < 0, 5, 5 >	// Kijk schuin 
//    location < 0, 0.5, 2 >	// Kijk laag 
    look_at < 0, 0, 0 >
}

#declare PuzzleBody = box {
    <-PuzzleWidth/2,0,-PuzzleHeight/2>
    <0.5*PuzzleWidth,PuzzleThickness,0.75*PuzzleHeight>
}

#declare Puzzle = difference {
    union {
	object { PuzzleBody }
	object { PuzzleHead translate <0,0,0.75*PuzzleHeight> }
    }
    object { PuzzleHead scale <1,1.1,1> translate <0,-Eps,-PuzzleHeight/2> }
    texture { S_Wood1 scale 2 rotate <3,3,0> }
}

#declare RotanPuzzle = union {
    object { Puzzle rotate y*90 }
    text {
	ttf nameFont "Rotan" LetterThickness, -0.03*x
	rotate x*90
	scale 0.7
	scale <1.2,1,1>
	translate <-0.2,PuzzleThickness+LetterThickness-Eps,-0.2>
	texture { nameTexture }
    }
}

object { RotanPuzzle translate <-0.7,0,0> }

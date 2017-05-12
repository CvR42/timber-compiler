#include "colors.inc"
#include "woods.inc"
#include "pascal.inc"
#include "puzzle.inc"

camera 
{
    right 200/200*x
    up y
    location < 0, 4.9, 0 >
//    location < 0, 5, 5 >	// Kijk schuin 
//    location < 0, 0.5, 2 >	// Kijk laag 
    look_at < 0, 0, 0 >
}

#declare PuzzleBody = box {
    <-0.75*PuzzleWidth,0,-PuzzleHeight/2>
    <0.5*PuzzleWidth,PuzzleThickness,PuzzleHeight/2>
}

#declare Puzzle = difference {
    union {
	object { PuzzleBody }
	object { PuzzleHead translate <0,0,PuzzleHeight/2> }
    }
    object { PuzzleHead scale <1,1.1,1> translate <0,-Eps,-PuzzleHeight/2> }
    object { PuzzleHead scale <1,1.1,1> rotate y*90 translate <-0.75*PuzzleWidth,-Eps,0> }
    texture { S_Wood1 scale 0.6 rotate <3,3,0> }
}

#declare TmPuzzle = union {
    object { Puzzle rotate y*180 }
    text {
	ttf ioFont "Template" LetterThickness, 0
	rotate x*90
	scale 0.38
	translate <-0.7,PuzzleThickness+LetterThickness-Eps,0.1>
	texture { ioTexture }
    }
    text {
	ttf nameFont "Tm" LetterThickness, 0
	rotate x*90
	scale 0.9
	scale <1.2,1,1>
	translate <-0.7,PuzzleThickness+LetterThickness-Eps,-1.2>
	texture { nameTexture }
    }
    text {
	ttf ioFont "Code" LetterThickness, 0
	rotate x*90
	scale 0.38
	translate <-0.4,PuzzleThickness+LetterThickness-Eps,-2.2>
	texture { ioTexture }
    }
    text {
	ttf ioFont "Domain" LetterThickness, 0
	rotate x*90
	rotate y*-90
	scale 0.38
	translate <1.2,PuzzleThickness+LetterThickness-Eps,-0.6>
	texture { ioTexture }
    }
}

object { TmPuzzle translate <-0.6,0,0.6> }

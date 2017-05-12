#include "colors.inc"
#include "woods.inc"
#include "pascal.inc"
#include "puzzle.inc"

camera 
{
    right 600/200*x
    up y
    location < 0, 4.9, 0 >
//    location < 0, 5, 5 >	// Kijk schuin 
//    location < 0, 0.5, 2 >	// Kijk laag 
    look_at < 0, 0, 0 >
}

#declare Jiggle = 0.02

#declare PuzzleBody = box {
    <-PuzzleWidth/2,0,-PuzzleHeight/2>
    <PuzzleWidth/2,PuzzleThickness,PuzzleHeight/2>
}

#declare Puzzle = difference {
    union {
	object { PuzzleBody }
	object { PuzzleHead             translate <0,0,PuzzleHeight/2> }
	object { PuzzleHead rotate y*90 translate <PuzzleWidth/2,-Eps,0> }
    }
    object { PuzzleHead scale <1.1,1.1,1.1> translate <0,-Eps,-PuzzleHeight/2> }
    object { PuzzleHead scale <1.1,1.1,1.1> rotate y*90 translate <-PuzzleWidth/2,-Eps,0> }
    texture { S_Wood1 scale 1 rotate <3,3,0> }
}

#declare Background = union {
    object { Puzzle translate <-3.6*PuzzleWidth,2*Jiggle,-1.5*PuzzleHeight> rotate y*9 }
    object { Puzzle translate <-5.2*PuzzleWidth,2*Jiggle,2.2*PuzzleHeight> rotate y*-22 }
    object { Puzzle translate <-5.0*PuzzleWidth,2*Jiggle,-1.8*PuzzleHeight> rotate y*60 }

    object { Puzzle translate <-3*PuzzleWidth, 0*Jiggle, 1*PuzzleHeight> }

    object { Puzzle translate <-2*PuzzleWidth, 0*Jiggle, 0*PuzzleHeight> }

    object { Puzzle translate <-1*PuzzleWidth, 1*Jiggle,-PuzzleHeight> }
    object { Puzzle translate <-1*PuzzleWidth, 0*Jiggle,PuzzleHeight> }
    object { Puzzle translate <-1*PuzzleWidth,-1*Jiggle,0> }

    object { Puzzle translate < 0*PuzzleWidth, 0*Jiggle, 0*PuzzleHeight> }

    object { Puzzle translate < 1*PuzzleWidth, 3*Jiggle,-1*PuzzleHeight> }
    object { Puzzle translate < 1*PuzzleWidth, 1*Jiggle, 0*PuzzleHeight> }
    object { Puzzle translate < 1*PuzzleWidth, 3*Jiggle, 1*PuzzleHeight> }

    object { Puzzle translate < 2*PuzzleWidth, 5*Jiggle, 0*PuzzleHeight> }
    object { Puzzle translate < 2*PuzzleWidth,-1*Jiggle, 1*PuzzleHeight> }
    object { Puzzle translate < 2*PuzzleWidth,-1*Jiggle,-1*PuzzleHeight> }

    object { Puzzle translate < 4*PuzzleWidth, 4*Jiggle, 0*PuzzleHeight> rotate y*23 }
    object { Puzzle translate <4.5*PuzzleWidth,0*Jiggle,-1.5*PuzzleHeight> rotate y*-12 }
    object { Puzzle translate < 6*PuzzleWidth, 2*Jiggle,1.5*PuzzleHeight> rotate y*40 }
    object { Puzzle translate < 6*PuzzleWidth, 2*Jiggle,0.5*PuzzleHeight> rotate y*-40 }
}

#declare Foreground = text {
    ttf nameFont "Timber" LetterThickness, -0.03*x
    rotate x*90
    scale 1
    scale <1.2,1,1>
    translate <-2,PuzzleThickness+LetterThickness-Eps,0.5>
    texture { nameTexture }
}

#declare Foreground2 = text {
    ttf nameFont "A Spar/Java compiler" LetterThickness, -0.03*x
    rotate x*90
    scale 1
    scale <1.2,1,1>
    translate <-5,PuzzleThickness+LetterThickness-Eps,-0.5>
    texture { nameTexture }
}

object { Foreground }
object { Foreground2 }
object { Background scale 0.38 }

public class DualAStarApproach {
/*

    A: number of nodes in the top layer that a node may branch out to
    B: radius(manhattan) of spaces

    At the top level, insert the start position as a node
    Greedily pick A neighbour lambdas within radius B
    Do multiple bottom layer A* search from the SP to the NP, limit them to C steps of depth and D steps of breadth, process the A* calculations interleaved to have an early out if say 1/3 of them have a solution


    The bottom layer a* search is a straigh pathfinding from SP to NP
    It is limited in depth and breadth, it may be stopped if other searches find results earlier.
    The open/closed set is keyed by a (sha1?) hash of the worldstates

    The point of the open/closed keying is to detect multiple paths to a point, and pick the on with the least steps.
*/

}

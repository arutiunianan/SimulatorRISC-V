#define BOARD_SIZE 20

int isSafe(int board[BOARD_SIZE][BOARD_SIZE], int row, int col)
{
    for (int x = 0; x < col; x++)
        if (board[row][x] == 1)
            return 0;
    for (int x = row, y = col; x >= 0 && y >= 0; x--, y--)
        if (board[x][y] == 1)
            return 0;
    for (int x = row, y = col; x < BOARD_SIZE && y >= 0; x++, y--)
        if (board[x][y] == 1)
            return 0;
    return 1;
}
 
int solveNQueens(int board[BOARD_SIZE][BOARD_SIZE], int col)
{
    if (col == BOARD_SIZE)
        return 1;

    for (int i = 0; i < BOARD_SIZE; i++) {
        if (isSafe(board, i, col)) {
            board[i][col] = 1;
            if (solveNQueens(board, col + 1))
                return 1;
            board[i][col] = 0;
        }
    }
    return 0;
}
 
int main()
{
    int board [BOARD_SIZE][BOARD_SIZE]; 
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++)
            board[i][j] = 0;
    }

    return solveNQueens(board, 0);
}
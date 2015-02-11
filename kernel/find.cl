__kernel void find(
	char* data,
	char* pattern,
	int patternRows,
	int patternCols,
	char* indices
)
{
	int row = get_global_id(0);
	int col = get_global_id(1);
	
	int indexCols = get_global_size(1);
	int dataCols = indexCols + patternCols - 1;
	
	char count = 0;
	
	for (int i = 0; i < patternRows; ++i)
	{
		for (int j = 0; j < patternCols; ++j)
		{
			if (row >= i && col >= j && pattern[i * patternCols + j] == data[(row+i) * dataCols + col+j])
			{
				count++;
			}
		}
	}
	indices[row * indexCols + col] = (count == patternRows * patternCols) ? 1 : 0;
}
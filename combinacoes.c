int Combinacoes(int n, int r)
{
    int i, combinacoes;

    if (n < r || r < 1)
	return 0;

    i = 1;
    combinacoes = 1;
    while (i <= r)
    {
	combinacoes *= (n - r + i);
	combinacoes /= i;
	i++;
    }
    return combinacoes;
}

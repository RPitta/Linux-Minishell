#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char buffer[3];
	buffer[2] = '\0';

	char *filename = argv[1];

	FILE *f = fopen(filename, "w");

	if (f == NULL)
	{
		printf("Error opening file\n");
		exit(1);
	}

	for (char c = 65; c < 91; c++)
	{
		buffer[0] = c;
		for (char d = 65; d < 91; d++)
		{
			buffer[1] = d;
			fprintf(f, "wget https://en.wikipedia.org/wiki/%s -O %s.html&\n", buffer, buffer);
		}
	}

	fprintf(f, "%s\n", "barrier");

	for (char c = 65; c < 91; c++)
	{
		buffer[0] = c;
		for (char d = 65; d < 91; d++)
		{
			buffer[1] = d;
			fprintf(f, "lynx -dump -nolist %s.html > %s.txt&\n", buffer, buffer);
		}
	}

	char grep_cmd[10240] = "grep -oh \"[a-zA-Z]*\" ";
	char letters[8];
	for (char c = 65; c < 91; c++)
	{
		for (char d = 65; d < 91; d++)
		{
			letters[7] = '\0';
			letters[0] = c;
			letters[1] = d;
			strcat(letters, ".txt ");
			strcat(grep_cmd, letters);
			memset(letters, 0, sizeof(letters));
		}
	}

	strcat(grep_cmd, "> allwords.txt\n");

	fprintf(f, "%s", grep_cmd);

	fprintf(f, "sort -o allwords_sorted.txt allwords.txt\n");
	fprintf(f, "uniq -ic allwords_sorted.txt count_uniqwords.txt\n");
	fprintf(f, "sort -k 1,1n count_uniqwords.txt\n");

	return 0;
} 
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define DWORD unsigned long long
#define EPSILON 0.00001f

/////////////////////////////
// Private Prototypes
bool open_file(const char *source_filename, FILE **source);
bool process_file(FILE *source);
DWORD find_source_size(FILE *source);

/////////////////////////////
// Public Functions
int main(int argc, char *argv[])
{
	int result = 20;

	if (argc != 2) 
	{
		printf("Usage: test source-filename.\n");
		result = 0;
	} 
	else
	{
		FILE *source;
		bool open_file_test;
		open_file_test = open_file(argv[1], &source);
		if (open_file_test) 
		{
			bool process_file_test;
			process_file_test = process_file(source);

			if (process_file_test)
			{
				fclose(source);
				result = 0;
			}
			else
			{
				result = 1;
			}
		}
		else
		{
			result = 1;
		}
	} 

	return result;
}

/////////////////////////////
// Private Functions
bool open_file(const char* source_filename,FILE **source)
{
	// read file
	bool result = false;

	*source = fopen(source_filename, "rb");
	if (*source != NULL) 
	{
		result = true;
	} 
	else
	{
		printf("Could not open source file for reading.\n");
	}

	return result;
}

bool process_file(FILE *source)
{
	bool result = true;

	int histogram[256] = {0};

	float previous_percentile = 0.0f;
	float diff_percentile = 0.0f;
	const float increment = 0.05f;

	DWORD source_size;
	source_size = find_source_size(source);

	unsigned char source_buffer[2048];

	DWORD amount_left;
	amount_left = source_size;

	printf("[");

	while (amount_left > 0)
	{
		int amount_read;
		amount_read = fread(source_buffer, sizeof(source_buffer[0]), sizeof(source_buffer), (FILE*)source);
		amount_left -= amount_read;
		
		int i;
		for(int i=0; i<amount_read; i++)
		{
			histogram[source_buffer[i]]++;
		}

		// progress printing
		{
			float current_percentile;

			// printf("%s %d\n", "amount_read", amount_read);
			current_percentile = (float)(source_size - amount_left) / source_size;
			// printf("%s %f\n", "current_percentile", current_percentile);
			diff_percentile += current_percentile - previous_percentile;
			// printf("%s %f\n", "diff_percentile", diff_percentile);
			
			bool flag = false;
			while ((diff_percentile + EPSILON) > increment)
			{
				printf("-");
				diff_percentile -= increment;
				flag = true;
			}

			if (flag) 
			{
				previous_percentile = current_percentile;
			}
		}

		if ((amount_read == 0) && (amount_left != 0))
		{
			result = false;
			break;
		}
	}

	printf("]\n");

	if (result)
	{
		int i;
		for(i=0; i<256; i++)
		{
			int y;
			int index;
			int largest;
	
			largest = -1;
			index = -1;
	
			for (y = 0; y < 256;y++)
			{
				if (histogram[y] > largest)
				{
					largest = histogram[y];
					index = y;
				}
			}

			if (index >= 0)
			{
				if (histogram[index] > 0)
				{
					printf("%c (%d) = %d\n", (char)index, index, histogram[index]);
					histogram[index] = -1;
				}
			}
		}
	}
	return result;
}

DWORD find_source_size(FILE *source)
{
	DWORD result;
	DWORD org_pos;

	org_pos = ftell(source);
	fseek(source, 0, SEEK_END);
	result = ftell(source);
	fseek(source, org_pos, SEEK_SET);

	return result;
}







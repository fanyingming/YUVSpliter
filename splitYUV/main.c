#include <stdlib.h>
#include <stdio.h>

void yv12_to_i420( unsigned char *yv12,unsigned char *i420, int width, int height )
{
	memcpy( i420, yv12, width*height);
	memcpy( i420+width*height, yv12+width*height+width*height/4, width*height/4 );
	memcpy( i420+width*height+width*height/4, yv12+width*height, width*height/4 );
}

void yuva_to_yuvyuv( unsigned char *input,unsigned char *output, int width, int height )
{
	//yuv
	memcpy( output, input, (width*height*3)/2 );
	input += (width*height*3)/2;
	output+= (width*height*3)/2;
	//Alpha
	memcpy( output, input, width*height );
	output+= width*height;
	//uv 
	memset( output, 0, (width*height)/2);
}
void resolution_split( unsigned char *input,unsigned char *output, int width_new, int height_new, int width, int height )
{
	int i;
	unsigned char *input_bak=input;
	for( i=0; i<height_new; i++ )
	{
		memcpy( output, input, width_new );
		input += width;
		output += width_new;
	}

	input = input_bak + width*height;
	for( i=0; i<height_new/2; i++ )
	{
		memcpy( output, input, width_new/2 );
		input += width/2;
		output += width_new/2;
	}

	input = input_bak + width*height + width*height/4;
	for( i=0; i<height_new/2; i++ )
	{
		memcpy( output, input, width_new/2 );
		input += width/2;
		output += width_new/2;
	}
}
void nv12_to_i420( unsigned char *nv12,unsigned char *i420, int width, int height )
{
	int i,j;
	memcpy( i420, nv12, width*height);
	i420+=width*height;
	nv12+=width*height;
	for(i=0,j=0;i<width*height/4;i++,j+=2)
	{
		i420[i] = nv12[j];
	}
	i420+=width*height/4;
	for(i=0,j=1;i<width*height/4;i++,j+=2)
	{
		i420[i] = nv12[j];
	}
}

int main( int argc, char **argv )
{
	char input_filename[200]/* = argv[1]*/;
	int frame_num/* = atoi(argv[2])*/;
	long seek;
	long width/* = atoi(argv[3])*/;
	long height/* = atoi(argv[4])*/;
	int width_new;
	int height_new;
	char output_filename[200]/* = argv[5]*/;
	int option;
	unsigned char *input_yuv;
	unsigned char *output_yuv;
	FILE *fin;
	FILE *fout;
 
	int i;

	printf("YUV split and converse tool. By Yingming. 2014 09 01 \n");
	printf("Useage: input output frames seek width height option(int).\n");
	printf("option:\n1.I420 to I420;\n2.YV12 to I420;\n3.NV12 to I420;\n4.resolution split;\n5.YUVA to 2 frame yuv.\n");
	printf("Please input.\n");
	scanf("%s %s %d %d %d %d %d",input_filename,output_filename,&frame_num,&seek,&width,&height,&option);
	if( option==4 )
	{
		printf("Please input width height.\n");
		scanf("%d %d",&width_new,&height_new);
		if( width_new>width || height_new>height )
		{
			printf("new width and height can't bigger than original width and height.\n");
			exit(0);
		}
	}
	fin = fopen( input_filename, "rb");
	fout= fopen( output_filename, "wb");
	

	if( option==5 )
	{
		input_yuv = (unsigned char *)malloc((width * height * 5) / 2);
		output_yuv = (unsigned char *)malloc((width * height * 3));
	}
	else
	{
		input_yuv = (unsigned char *)malloc((width * height * 3) / 2);
		output_yuv = (unsigned char *)malloc((width * height * 3) / 2);
	}
	if( seek )
		fseek(fin,seek*(width * height * 3) / 2,SEEK_SET);
	for( i=1; i<=frame_num; i++ ) {
		if( option==5 )
			fread( input_yuv, 1, (width * height * 5) / 2, fin );
		else
			fread( input_yuv, 1, (width * height * 3) / 2, fin );

		switch(option)
		{
		case 1 : 
			{
				fwrite( input_yuv, 1, (width * height * 3) / 2, fout );
				break;
			}
		case 2 : 
			{
				yv12_to_i420( input_yuv, output_yuv, width, height );
				fwrite( output_yuv, 1, (width * height * 3) / 2, fout );
				break;
			}
		case 3 : 
			{
				nv12_to_i420( input_yuv, output_yuv, width, height );
				fwrite( output_yuv, 1, (width * height * 3) / 2, fout );
				break;
			}
		case 4:
			{
				resolution_split( input_yuv, output_yuv, width_new, height_new, width, height );
				fwrite( output_yuv, 1, (width_new * height_new * 3) / 2, fout );
				break;
			}
		case 5:
			{
				yuva_to_yuvyuv( input_yuv, output_yuv,  width, height );
				fwrite( output_yuv, 1, (width * height * 3) , fout );
				break;
			}
		}
		printf("output %d/%d frame.\n",i,frame_num);
		fflush( fout );
	}

	fclose(fout);
	fclose(fin);

	free(input_yuv);
	free(output_yuv);
}
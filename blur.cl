__kernel void blur  (
    const __constant unsigned char * in, 
    unsigned __global char * out, 
    const int altura, 
    const int largura, 
    const int tamanho_mascara){

        int col = get_global_id(1);
        int row = get_global_id(0);
        int curRow;
        int curCol;
        int pixVal;
        int pixels;
        int blurRow;
        int blurCol;


        if(col < largura && row < altura){
            pixVal = 0;
            pixels = 0;
            
            for(blurRow = -tamanho_mascara; blurRow< tamanho_mascara+1; blurRow++){
                for (blurCol = -tamanho_mascara; blurCol< tamanho_mascara+1; blurCol++)

                curRow = row + blurRow;
                curCol = col + blurCol;
                
                if (curRow > -1 && curRow < largura && curCol > -1 && curCol < altura){
                    pixVal+= in[curRow*altura + curCol];
                    pixels++;
                }
            
                out[row*altura + col] = (unsigned char) (pixVal/pixels);
            }

        }

    }
/* miniEP1
 * Nome: Ísis Ardisson Logullo
 * nUSP: 7577410
 * java miniep1.java
 */

import java.util.*;


public class miniep1{
    public static void main(String[] args) {
        double n = 1;
        int i, j, k = 0;
        double bitshift = 0;
        int primos = 0;
        int primosEspeciais = 0;
        

        System.out.println("Digite um numero: ");
        Scanner sc = new Scanner(System.in);
        bitshift = sc.nextDouble();
        sc.close();
        n = n * Math.pow(2,bitshift);
        
        boolean[] crivo = new boolean[(int)n+1];

        for (i = 2; i < n+1; i++){
            crivo[i] = true;
        }
        for (i = 2; i < n+1; i++){
            k = i;
            if (crivo[i] == true){
                primos++;
                if (i % 4 != 3){
                    primosEspeciais++;
                }
                for (j = i*2; j < n+1; k++){
                    crivo[j] = false;
                    j = j + k ;
                }
            }
        }


        System.out.printf("%d %d\n", primos, primosEspeciais);

    }
}

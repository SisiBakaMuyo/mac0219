from time import sleep
from tqdm import tqdm
import subprocess
regiaoA = {
  "0": "-0.188 -0.012 0.554 0.754",
  "1": "0.175 0.375 -0.1 0.1",
  "2": "-0.8 -0.7 0.05 0.15",
  "3": "-2.5 1.5 -2.0 2.0"
}
with tqdm(total=2400) as pbar:
    for regiao in range(4):
        for thread in range(6):
            for tamanho in range(10):
                for execu in range(10):
                    PTH = "./mandelbrot_pth " + " " + regiaoA[str(regiao)] + " " + str(2**(tamanho+4)) + " " + str(2**(thread)) + ' "' + ";PTH;" + str(regiao) + ";" + str(2**(tamanho+4)) + ";" + str(2**(thread)) + ";" + str(execu) + '"' + " >> ./log.csv"
                    OMP = "./mandelbrot_omp " + " " + regiaoA[str(regiao)] + " " + str(2**(tamanho+4)) + " " + str(2**(thread)) + ' "' + ";OMP;" + str(regiao) + ";" + str(2**(tamanho+4)) + ";" + str(2**(thread)) + ";" + str(execu) + '"' + " >> ./log.csv"
                    subprocess.check_call(OMP, shell=True)                  
                    subprocess.check_call(PTH, shell=True)        
                    pbar.update()
                    
from glob import glob

arquivos = glob("*.launchd.xml")
for arquivo in arquivos:
    fin = open(arquivo, "rt")
    data = fin.read()
    #data = data.replace('../SUMO-Manhattan-1km_x_1km/', 'C:\\\\veins-veins-5.1\\\\examples\\\\grid_seeds\\\\SUMO-Manhattan-1km_x_1km\\\\')
    data = data.replace('../SUMO-Manhattan-1km_x_1km/', '/home/jordan/git/Flexe2.0.0/simulations/grid/seeds')
    fin.close()
    fin = open(arquivo, "wt")
    fin.write(data)
    fin.close()


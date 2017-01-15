with open('./assets/level/level0.level', 'w+') as f:
  level_name = input("Level name: ")
  f.write(level_name + "\n")
  w = int(input("Width: "))
  h = int(input("Height: "))
  f.write(str(w) + "x" + str(h) + "\n")
  # Defaults to 32x32 for now
  f.write(str(32) + "x" + str(32) + "\n")
  for i in range(h):
    for j in range(w-1):
      f.write('0:0 ')
    f.write('0:0\n')
  f.write("assets/gfx/spritesheet.png\n")
  f.write("assets/gfx/editor_background.png\n")
   

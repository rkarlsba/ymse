// vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
//
// Please see README.md

#include <assert.h>    // just to make sure
#include <unistd.h>    // To write messages on stderr
#include <stdio.h>     // to read from stdin and write to stdout

char buffer[84];       // to read the header
struct triangle {      // to read all the data in one go
  float sommet[4][3];
  short abc __attribute__((packed));
} __attribute__((packed)) triangles[16*1024*1024];


    int main(void) {
      // trouble shooting info
      fprintf(stderr, "sizeof (short) = %ld, sizeof (int) = %ld, sizeof (float) = %ld, sizeof (struct triangle) = %ld\n", 
              sizeof (short), sizeof (int), sizeof (float), sizeof (struct triangle));
      // Make sure the data has the expected size
      assert(sizeof (short) == 2);
      assert(sizeof (int) == 4);
      assert(sizeof (float) == 4);
      assert(sizeof (struct triangle) == 50);
    
      // read the whole file
      int l=read(0, buffer, 84);
      fprintf(stderr, "read l=%d bytes\n", l);
      assert(l==84);
      fprintf(stderr, "%d triangles\n", ((int*)buffer)[20]);

      int i; 
      for (i=0; i<=20; i++) fprintf(stderr, "%08X ", ((int*)buffer)[i]);
    
      l=read(0, triangles, 1024*1024*50+1);
      assert(l%50==0);
    

      // find min and max values in the 3 dimensions (we dont care about the normal vector in sommet[0])          int j, n=l/50;
      fprintf(stderr, "%d records read\n", n);
      float minx, maxx, miny, maxy, minz, maxz;
    
      minx = maxx = triangles[0].sommet[1][0];
      miny = maxy = triangles[0].sommet[1][1];
      minz = maxz = triangles[0].sommet[1][2];
    
      for (i = 0; i<n; i++)
        for (j = 1; j<=3; j++) {
/*        troubleshooting messages
          fprintf(stderr, "%5d, %d: ", i, j);
          fprintf(stderr, "%08X %c ", ((int*)triangles[i].sommet[j])[0], 'A');
          fprintf(stderr, "%08X %c ", ((int*)triangles[i].sommet[j])[1], 'B');
          fprintf(stderr, "%08X %c ", ((int*)triangles[i].sommet[j])[2], 'C');  
          fprintf(stderr, "%f %c ", triangles[i].sommet[j][0], 'D');
          fprintf(stderr, "%f %c ", triangles[i].sommet[j][1], 'E');
          fprintf(stderr, "%f %c %s\n", triangles[i].sommet[j][2], 'F', "test"); */
          if (triangles[i].sommet[j][0]<minx) minx = triangles[i].sommet[j][0];
          if (triangles[i].sommet[j][0]>maxx) maxx = triangles[i].sommet[j][0];
          if (triangles[i].sommet[j][1]<miny) miny = triangles[i].sommet[j][1];
          if (triangles[i].sommet[j][1]>maxy) maxy = triangles[i].sommet[j][1];
          if (triangles[i].sommet[j][2]<minz) minz = triangles[i].sommet[j][2];
          if (triangles[i].sommet[j][2]>maxz) maxz = triangles[i].sommet[j][2];
        }
      // compute midpoint and substract them from all coordinates (except for normal vector which is not related with object position
      float midx = (minx+maxx)/2, midy = (miny+maxy)/2, lowz = minz;
    
      for (i = 0; i<n; i++)
        for (j = 1; j<=3; j++) {
          triangles[i].sommet[j][0] -= midx;
          triangles[i].sommet[j][1] -= midy;
          triangles[i].sommet[j][2] -= lowz;
        }

      // Print summary info
      fprintf(stderr, "minx = %f, maxx = %f\nminy = %f, maxy = %f\nminz = %f, maxz = %f\n", minx, maxx, miny, maxy, minz, maxz);

      // write result on stdout
      assert(write(1, buffer, 84)==84);
      assert(write(1, triangles, l)==l);
      return 0;
    }

/*** Author :Vibhav Gogate
The University of Texas at Dallas
*****/


import java.awt.AlphaComposite;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;

/* Customized import packages */
import java.lang.Math;
import java.util.Arrays;


public class KMeans {
    public static void main(String [] args){
	if (args.length < 3){
	    System.out.println("Usage: Kmeans <input-image> <k> <output-image>");
	    return;
	}
	try{
	    BufferedImage originalImage = ImageIO.read(new File(args[0]));
	    int k=Integer.parseInt(args[1]);
	    BufferedImage kmeansJpg = kmeans_helper(originalImage,k);
	    ImageIO.write(kmeansJpg, "jpg", new File(args[2]));

	}catch(IOException e){
	    System.out.println(e.getMessage());
	}
    }

    private static BufferedImage kmeans_helper(BufferedImage originalImage, int k){
	int w=originalImage.getWidth();
	int h=originalImage.getHeight();
	BufferedImage kmeansImage = new BufferedImage(w,h,originalImage.getType());
	Graphics2D g = kmeansImage.createGraphics();
	g.drawImage(originalImage, 0, 0, w,h , null);
	// Read rgb values from the image
	int[] rgb=new int[w*h];
	int count=0;
	for(int i=0;i<w;i++){
	    for(int j=0;j<h;j++){
		rgb[count++]=kmeansImage.getRGB(i,j);
	    }
	}
	// Call kmeans algorithm: update the rgb values
	kmeans(rgb,k);

	// Write the new rgb values to the image
	count=0;
	for(int i=0;i<w;i++){
	    for(int j=0;j<h;j++){
		kmeansImage.setRGB(i,j,rgb[count++]);
	    }
	}
	return kmeansImage;
    }

    // Your k-means code goes here
    // Update the array rgb by assigning each entry in the rgb array to its cluster center
    private static void kmeans(int[] rgb, int k){
        int a[] = new int[rgb.length]; /* assignments */
        int c[] = new int[k]; /* center */
        int nc[] = new int[k]; /* new center array */
        int rc,gc,bc,nrc,ngc,nbc;
        for(int i=0;i<rgb.length;i++){
            a[i] = (int)(Math.random() * k);
        }
        int j;
        update_means(rgb, a, c);
        for(int i=0;i<200;i++){
            update_assignments(rgb, a, c);
            nc = Arrays.copyOf(c, c.length);
            update_means(rgb, a, c);
            // check the change of center
            for(j=0;j<c.length;j++){
                rc = c[j] & 0x00ff0000;
                rc = rc>>16;
                gc = c[j] & 0x0000ff00;
                gc = gc>>8;
                bc = c[j] & 0x000000ff;
                nrc = nc[j] & 0x00ff0000;
                nrc = nrc>>16;
                ngc = nc[j] & 0x0000ff00;
                ngc = ngc>>8;
                nbc = c[j] & 0x000000ff;
                if(rc!=nrc||gc!=ngc||bc!=nbc)
                    break;
            }
            if(j==c.length)
                break;
        }
        for(int i=0;i<rgb.length;i++){
            rgb[i] = c[a[i]];
        }
    }

    private static void update_means(int[] rgb, int[] a, int [] c){
        int r,g,b;
        int rc[] = new int[c.length];
        int gc[] = new int[c.length];
        int bc[] = new int[c.length];
        int len[] = new int[c.length];
        for(int i=0;i<c.length;i++){
            rc[i] = 0;
            gc[i] = 0;
            bc[i] = 0;
            len[i] = 0;
        }
        for(int i=0;i<rgb.length;i++){
            r = rgb[i] & 0x00ff0000;
            r = r>>16;
            g = rgb[i] & 0x0000ff00;
            g = g>>8;
            b = rgb[i] & 0x000000ff;
            rc[a[i]] = rc[a[i]] + r;
            gc[a[i]] = gc[a[i]] + g;
            bc[a[i]] = bc[a[i]] + b;
            len[a[i]] = len[a[i]] + 1;
        }
        for(int i=0;i<c.length;i++){
            c[i] = 0xff000000 | ((rc[i]/len[i])<<16) | ((gc[i]/len[i])<<8) | ((bc[i])/len[i]);
        }
    }

    private static void update_assignments(int[] rgb, int[] a, int [] c){
        int r,g,b;
        int rc[] = new int[c.length];
        int gc[] = new int[c.length];
        int bc[] = new int[c.length];
        for(int i=0;i<c.length;i++){
            rc[i] = c[i] & 0x00ff0000;
            rc[i] = rc[i]>>16;
            gc[i] = c[i] & 0x0000ff00;
            gc[i] = gc[i]>>8;
            bc[i] = c[i] & 0x000000ff;
        }
        for(int i=0;i<rgb.length;i++){
            r = rgb[i] & 0x00ff0000;
            r = r>>16;
            g = rgb[i] & 0x0000ff00;
            g = g>>8;
            b = rgb[i] & 0x000000ff;
            for(int j=0;j<c.length;j++){
                if((r-rc[j]) * (r-rc[j]) + (g-gc[j]) * (g-gc[j]) + (b-bc[j]) * (b-bc[j]) < (r-rc[a[i]]) * (r-rc[a[i]]) + (g-gc[a[i]]) * (g-gc[a[i]]) + (b-bc[a[i]]) * (b-bc[a[i]])){
                    a[i] = j;
                }
            }
        }
    }
}

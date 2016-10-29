package model;

/**
 * Created by Sumner on 9/20/16.
 */
public class CreatureShell {
    private int id, energy, health, fam, gen;
    private double x,y,d,app;

    private String braintype = "N/A";
    private double[] inputs = null;
    private double[] outputs = null;

    CreatureShell(int id, int fam, double x, double y, double d, double app){
        this.id = id; this.fam = fam; this.gen = -1;
        this.x = x; this.y = y; this.d = d; this.app = app;
    }
    CreatureShell(int id, int fam, int gen, double x, double y, double d, double app, int energy, int health,
                         String braintype, double[] inputs, double[] outputs){
        this(id,fam,x,y,d,app);
        this.gen = gen;
        this.energy = energy; this.health = health;
        this.braintype = braintype; this.inputs = inputs; this.outputs = outputs;

    }

    public double getX(){return x;}
    public double getY(){return y;}
    public double getD(){return d;}
    public int getID() {return id;}
    public String getBraintype(){return braintype;}
    public double[] getInputs(){return inputs;}
    public double[] getOutputs(){return outputs;}

    public int getHealth() {return health;}
    public int getEnergy() {return energy;}

    public int getFam() {
        return fam;
    }

    public int getGen() {
        return gen;
    }

    public double getApp() {
        return app;
    }
}

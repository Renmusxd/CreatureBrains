package model;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import java.io.*;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Map;

/**
 * Created by Sumner on 9/20/16.
 */
public class Model {

    private static final String STATE_MSG = "STATE";
    private static final String RATE_MSG = "GETRATE";
    private static final String CRINFO_MSG = "CRINFO";
    private static final String FOOD_MSG = "FOOD";

    private static final String NO_CREATURE = "No such creature";

    private Socket con;
    private BufferedReader sin;
    private PrintWriter sout;
    private double rate = 60;

    private int height = 100, width = 100;

    private double creatures_updatetime = 0;
    private final ArrayList<CreatureShell> creatures;

    private double food_updatetime = 0;
    private int maxfood = 0;
    private int[] foodsquares = null;
    private int xsquares = 0;
    private int ysquares = 0;

    private double selc_updatetime = 0;
    private CreatureShell sel_c;

    public Model(String addr, int port){
        creatures = new ArrayList<>();
        try {
            con = new Socket(addr, port);
            sin = new BufferedReader(new InputStreamReader(con.getInputStream()));
            sout = new PrintWriter(con.getOutputStream());
            sin.readLine(); // Greeting

            update();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void update(){
        updateState();
        updateFood();
    }


    public ArrayList<CreatureShell> getCreatures(){
        synchronized (creatures) {
            return creatures;
        }
    }

    public int getXSquares(){return xsquares;}
    public int getYSquares(){return ysquares;}
    public int getMaxFood(){return maxfood;}
    public int getFood(int xindx, int yindx){
        if (xindx>=0 && xindx<xsquares && yindx>=0 && yindx<ysquares){
            return foodsquares[(ysquares*yindx) + xindx];
        }
        return -1;
    }

    public int getHeight(){
        return height;
    }
    public int getWidth(){
        return width;
    }

    public void updateFood(){
        if (dueForUpdate(food_updatetime)) {
            String sfood = sendResp(FOOD_MSG);
            try {
                JSONParser parser = new JSONParser();
                JSONObject obj = (JSONObject) parser.parse(sfood);
                maxfood = ((Long) obj.get("max")).intValue();
                xsquares = ((Long) obj.get("nx")).intValue();
                ysquares = ((Long) obj.get("ny")).intValue();
                foodsquares = new int[ysquares * xsquares];
                JSONArray jfood = (JSONArray) obj.get("food");
                int i = 0;
                for (Object jfobj : jfood) {
                    foodsquares[i++] = ((Long) jfobj).intValue();
                }
            } catch (ParseException e) {
                xsquares = 1;
                ysquares = 1;
                maxfood = 1;
                foodsquares = new int[ysquares * xsquares];
                foodsquares[0] = maxfood;
            }
        }
    }

    public CreatureShell getCritterInfo(int id) {
        if (sel_c==null || sel_c.getID()!=id || dueForUpdate(selc_updatetime)){
            String sselc = sendResp(CRINFO_MSG+" "+id);
            if (sselc!=null && !NO_CREATURE.equals(sselc)){
                try {
                    JSONParser parser = new JSONParser();
                    JSONObject obj = (JSONObject) parser.parse(sselc);
                    if (obj!=null) {
                        int fam = Math.toIntExact((long)obj.get("fam"));
                        int gen = Math.toIntExact((long)obj.get("gen"));
                        double x = (double) obj.get("x");
                        double y = (double) obj.get("y");
                        double d = (double) obj.get("d");
                        double app = (double) obj.get("app");
                        int energy = ((Long) obj.get("energy")).intValue();
                        int health = ((Long) obj.get("health")).intValue();
                        String braintype = (String) obj.get("braintype");
                        JSONArray jinputs = (JSONArray) obj.get("inputs");
                        double[] inputs = new double[jinputs.size()];
                        int i = 0;
                        for (Object jinobj : jinputs) {
                            try {
                                inputs[i++] = (Double) jinobj;
                            } catch (ClassCastException e){
                                inputs[i++] = ((Long) jinobj).doubleValue();
                            }
                        }

                        JSONArray joutputs = (JSONArray) obj.get("outputs");
                        double[] outputs = new double[joutputs.size()];
                        i = 0;
                        for (Object joutobj : joutputs) {
                            try{
                                outputs[i++] = (Double) joutobj;
                            } catch (ClassCastException e){
                                inputs[i++] = ((Long) joutobj).doubleValue();
                            }
                        }
                        sel_c = new CreatureShell(id,fam,gen, x, y, d,app, energy, health, braintype, inputs, outputs);
                    } else {
                        System.out.println(sselc);
                        sel_c = null;
                    }
                } catch (ParseException e){
                    System.out.println(sselc);
                    sel_c = null;
                } catch (NullPointerException e){
                    System.err.println("[!] NULL POINTER EXCEPTION: "+sselc);
                    System.err.println(" ---> "+CRINFO_MSG+" "+id);
                }
            } else {
                sel_c = null;
            }
        }
        return sel_c;
    }

    private void updateState(){
        synchronized (creatures) {
            if (dueForUpdate(creatures_updatetime)) {
                String statestr = sendResp(STATE_MSG);
                try {
                    JSONParser parser = new JSONParser();
                    JSONObject obj = (JSONObject) parser.parse(statestr);
                    height = Math.toIntExact((long)obj.get("height"));
                    width = Math.toIntExact((long) obj.get("width"));
                    JSONArray crs = (JSONArray) obj.get("creatures");

                    creatures.clear();
                    for (Object cobj : crs){
                        JSONObject cr = (JSONObject)cobj;
                        int id = Math.toIntExact((long)cr.get("id"));
                        int fam = Math.toIntExact((long)cr.get("fam"));
                        double x =(double)cr.get("x");
                        double y =(double)cr.get("y");
                        double d =(double)cr.get("d");
                        double app =(double)cr.get("app");
                        creatures.add(new CreatureShell(id,fam,x,y,d,app));
                    }
                } catch (ParseException e) {
                    System.err.println("[!] Error parsing state: "+e);
                    System.err.println(statestr);
                }
                creatures_updatetime = System.currentTimeMillis();

            }
        }
    }

    private synchronized String sendResp(String msg){
        try {
            sout.println(msg);
            sout.flush();
            String s = sin.readLine();
            if (s!=null && s.length()>0 && s.charAt(0)==0){return s.substring(1);}
            else {return s;}
        } catch (IOException e){
            return null;
        }
    }

    private boolean dueForUpdate(double last){
        double curr = System.currentTimeMillis();
        double delt = 0;
        if (rate!=0) {
          delt = 1 / rate;
        }
        return (last + delt < curr);
    }

}

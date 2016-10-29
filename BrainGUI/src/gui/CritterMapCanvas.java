package gui;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import model.CreatureShell;
import model.Model;

import java.util.ArrayList;

/**
 * Created by Sumner on 9/20/16.
 */
public class CritterMapCanvas extends ResizableCanvas {

    final static double CREATURE_SIZE = 10;
    final static int INFO_SIZE = 250;
    final static double SELECT_RANGE_SQUARED = 2500;
    final static double SELECT_AREA_SIZE = 30;
    final static double VIEW_LEN = 50;
    final static double VIEWANGLE = Math.PI / 4.0;

    private Model m;
    private CreatureShell sel_c;
    private int map_height;
    private int map_width;
    private double map_zoom = 1.0;

    private double zoom = 1.0;
    private double xoff = 0;
    private double yoff = 0;
    private int xmov = 0;
    private int ymov = 0;
    private double scrollspeed = 5;

    private double base_r;
    private double base_g;
    private double base_b;
    private double color_rslope;
    private double color_gslope;
    private double color_bslope;

    CritterMapCanvas(Model m){
        this.m = m;
        updateSize();
        this.setFocusTraversable(true);

        this.setOnMouseClicked(event -> {
            double mapx = screenXToMap(event.getSceneX());
            double mapy = screenYToMap(event.getSceneY());
            double mind2 = SELECT_RANGE_SQUARED;
            ArrayList<CreatureShell> critters = m.getCreatures();
            for (CreatureShell cs : critters){
                double d2 = Math.pow(mapx - cs.getX(),2) + Math.pow(mapy - cs.getY(),2);
                if (d2<SELECT_RANGE_SQUARED && d2<mind2){
                    mind2 = d2;
                    sel_c = cs;
                }
            }
        });
        this.setOnScroll(event -> {
            if (event.getDeltaY()<0){
                zoom *= 1.05;
                if (zoom>=10.0){zoom = 10.0;}
            } else {
                zoom *= 0.95;
                if (zoom <= 0.9){zoom = 0.9 ;}
            }
        });
        this.setOnKeyPressed(event -> {
            System.out.println(event.getCode());
            switch (event.getCode()){
                case UP:
                case W: {
                    ymov = -1;
                } break;
                case DOWN:
                case S: {
                    ymov = 1;
                } break;
                case LEFT:
                case A: {
                    xmov = -1;
                } break;
                case RIGHT:
                case D: {
                    xmov = 1;
                } break;
            }
        });
        this.setOnKeyReleased(event -> {
            switch (event.getCode()){
                case UP:
                case W: {
                    ymov = 0;
                } break;
                case DOWN:
                case S: {
                    ymov = 0;
                } break;
                case LEFT:
                case A: {
                    xmov = 0;
                } break;
                case RIGHT:
                case D: {
                    xmov = 0;
                } break;
            }
        });

        base_r = 0.4;
        base_g = 0.2;
        base_b = 0;
        double end_r = 0.2;
        double end_g = 0.4;
        double end_b = 0;

        color_rslope = end_r - base_r;
        color_gslope = end_g - base_g;
        color_bslope = end_b - base_b;
    }

    private void updateSize(){
        map_height = m.getHeight();
        map_width = m.getWidth();
        double xzoom = (getWidth()-INFO_SIZE)/map_width;
        double yzoom = getHeight()/map_height;
        if (xzoom>yzoom){map_zoom = yzoom;}
        else {map_zoom = xzoom;}

        xoff += xmov*scrollspeed;
        yoff += ymov*scrollspeed;
    }

    @Override
    void draw() {
        updateSize();
        GraphicsContext gc = getGraphicsContext2D();
        // Draw backdrop
        gc.setFill(Color.DARKBLUE);
        gc.fillRect(0,0,getWidth(),getHeight());

        // Draw creatures
        ArrayList<CreatureShell> critters = m.getCreatures();
        double tl_x = mapXToScreen(0);
        double tl_y = mapYToScreen(0);
        double br_x = mapXToScreen(map_width);
        double br_y = mapYToScreen(map_height);
        double sw = br_x - tl_x;
        double sh = br_y - tl_y;
        double half = CREATURE_SIZE/2;
        double scalesize = scaleDistance(CREATURE_SIZE);

        // Fill in food colors
        int maxfood = m.getMaxFood();
        int xsqrs = m.getXSquares();
        int ysqrs = m.getYSquares();

        for (int y = 0; y<ysqrs; y++){
            for (int x = 0; x<xsqrs; x++){
                int food = m.getFood(x,y);
                if (food>0) {
                    double screenx = ((double) x / xsqrs) * sw + tl_x;
                    double screeny = ((double) y / ysqrs) * sh + tl_y;
                    double dsx = (1.0 / xsqrs) * sw;
                    double dsy = (1.0 / ysqrs) * sh;
                    double foodrat = m.getFood(x, y) / (double) maxfood;

                    gc.setFill(new Color(
                            color_rslope * foodrat + base_r,
                            color_gslope * foodrat + base_g,
                            color_bslope * foodrat + base_b, 1));
                    gc.fillRect(screenx, screeny, dsx, dsy);
                    gc.setStroke(Color.BLACK);
                    gc.strokeRect(screenx, screeny, dsx, dsy);
                }
            }
        }
        for (CreatureShell cs : critters) {
            double larrowx = VIEW_LEN * Math.cos(cs.getD() - VIEWANGLE);
            double larrowy = VIEW_LEN * Math.sin(cs.getD() - VIEWANGLE);
            double carrowx = VIEW_LEN * Math.cos(cs.getD());
            double carrowy = VIEW_LEN * Math.sin(cs.getD());
            double rarrowx = VIEW_LEN * Math.cos(cs.getD() + VIEWANGLE);
            double rarrowy = VIEW_LEN * Math.sin(cs.getD() + VIEWANGLE);
            int f = cs.getFam();
            gc.setFill(new Color((Math.abs(("a" + f).hashCode()) % 255) / 255.0,
                    (Math.abs(("b" + f).hashCode()) % 255) / 255.0,
                    (Math.abs(("c" + f).hashCode()) % 255) / 255.0,
                    1));
            gc.setStroke(Color.BLACK);
            gc.fillOval(mapXToScreen(cs.getX() - half), mapYToScreen(cs.getY() - half), scalesize, scalesize);
            gc.strokeLine(mapXToScreen(cs.getX()), mapYToScreen(cs.getY()),
                    mapXToScreen(cs.getX() + carrowx), mapYToScreen(cs.getY() + carrowy));
            gc.setStroke(Color.GRAY);
            gc.strokeLine(mapXToScreen(cs.getX()), mapYToScreen(cs.getY()),
                    mapXToScreen(cs.getX() + larrowx), mapYToScreen(cs.getY() + larrowy));
            gc.strokeLine(mapXToScreen(cs.getX()), mapYToScreen(cs.getY()),
                    mapXToScreen(cs.getX() + rarrowx), mapYToScreen(cs.getY() + rarrowy));
            if (sel_c != null && cs.getID() == sel_c.getID()) {
                sel_c = cs;
            }
        }

        if (sel_c != null) {
            double selhalf = SELECT_AREA_SIZE / 2;
            double selscalesize = scaleDistance(SELECT_AREA_SIZE);
            gc.setStroke(Color.BLUE);
            gc.strokeOval(mapXToScreen(sel_c.getX() - selhalf), mapYToScreen(sel_c.getY() - selhalf), selscalesize, selscalesize);
        }
        // Now critter info on the right
        gc.setFill(Color.GRAY);
        gc.fillRect(getWidth() - INFO_SIZE, 0, INFO_SIZE, getHeight());
        gc.setFill(Color.BLACK);
        if (sel_c != null) {
            sel_c = m.getCritterInfo(sel_c.getID());
            if (sel_c != null) {
                final double X_OFF = 20;
                final double Y_OFF = 20;
                final double TEXT_SPACE = 15;
                final double DOUBLE_LEN = 100;
                int textiter = 0;
                int markiter = 0;
                int restore = 0;

                double infox = getWidth() - INFO_SIZE + X_OFF;

                gc.fillText("ID: " + sel_c.getID(),
                        infox, Y_OFF + (TEXT_SPACE * (textiter++)));

                markiter = textiter;

                gc.fillText("FAM: " + sel_c.getFam(),
                        infox, Y_OFF + (TEXT_SPACE * (textiter++)));

                gc.fillText("GEN: " + sel_c.getGen(),
                        infox, Y_OFF + (TEXT_SPACE * (textiter++)));

                gc.fillText("Health: " + sel_c.getHealth(),
                        infox, Y_OFF + (TEXT_SPACE * (textiter++)));

                gc.fillText("Energy: " + sel_c.getEnergy(),
                        infox, Y_OFF + (TEXT_SPACE * (textiter++)));

                restore = textiter;
                textiter = markiter;

                gc.fillText("App: " + sel_c.getApp(),
                        infox + DOUBLE_LEN, Y_OFF + (TEXT_SPACE * (textiter++)));

                textiter = restore;
                markiter = textiter;

                gc.fillText("Input: ",
                        infox, Y_OFF + (TEXT_SPACE * (textiter++)));

                for (double in : sel_c.getInputs()) {
                    gc.fillText("" + in,
                            infox, Y_OFF + (TEXT_SPACE * (textiter++)));
                }

                textiter = markiter;

                gc.fillText("Output: ",
                        infox + DOUBLE_LEN, Y_OFF + (TEXT_SPACE * (textiter++)));

                for (double out : sel_c.getOutputs()) {
                    gc.fillText("" + out,
                            infox + DOUBLE_LEN, Y_OFF + (TEXT_SPACE * (textiter++)));
                }
            }
        }
    }

    private double mapXToScreen(double x){
        return (x-xoff)*(map_zoom*zoom);
    }
    private double mapYToScreen(double y){
        return (y-yoff)*(map_zoom*zoom);
    }
    double screenXToMap(double x){
        return x/(map_zoom*zoom) + xoff;
    }
    double screenYToMap(double y){
        return y/(map_zoom*zoom) + yoff;
    }
    private double scaleDistance(double d){
        return map_zoom*zoom*d;
    }
}

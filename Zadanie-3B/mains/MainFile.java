package mains;

import java.util.*;

import customType.*;

public class MainFile {
	public final static int MEMORY_CELL_COUNT = 64;
	public final static int STARTING_MEMORY_CELL_COUNT = 32;
	public final static int MAX_STEP_COUNT = 500;
	public final static int INDIVIDUAL_COUNT = 50;
	public final static int GENERATION_COUNT = 200;
	public final static int TREASURE_COUNT = 7;
	public final static int MAX_VALUES = 255;
	public final static double ELITARISM_RATE = 0.1;
	public final static double NEW_INDIVIDUAL_RATE = 0.3;
	public final static double MUTATION_RATE = 0.6;
	
	public static void main(String[] args) {
		CustomVector mapSize = new CustomVector(7,7);
		CustomVector startingPosition = new CustomVector(0,3);
		Map map = new Map(mapSize, new CustomVector(1,2),
			new CustomVector(2,4), new CustomVector(4,1),
			new CustomVector(4,5), new CustomVector(6,3));
		Evolution evolution = new Evolution();
		
		/* Vytvorenie �vodnej gener�cie */
		List<StepSequence> individuals = Genetics.initialize(INDIVIDUAL_COUNT);
		
		/* Tvorenie nasleduj�cich gener�ci� */
		for (int j = 0; j < GENERATION_COUNT; j++) {
			for (StepSequence i : individuals) {
				if (i.isExecuted() == false) {
					evolution.execute(i, mapSize);
				}
				if (i.isTracked() == false) {
					map.findTreasures(i, new CustomVector(startingPosition.getX(), startingPosition.getY()));
				}
			}
			
			Evolution.output(individuals, j);
			
			/* Vytvorenie novej gener�cie jednotlivcov */
			individuals = Genetics.createNewGeneration(individuals);
		}
	}
	
	/**
	 * Zmeni ��slo na bin�rne - do Stringu
	 * @param number menen� ��slo
	 * @return bin�rne ��slo, reprezentovan� Stringom
	 */
	public static String getBytesOfInt(int number){
		return String.format("%8s", Integer.toBinaryString(number & 0xFF)).replace(' ', '0');
	}
	
	/**
	 * Zmen� bin�rne ��slo reprezentovan� Stringom sp� na integer
	 * @param val bin�rne ��slo, reprezentovan� Stringom
	 * @return zmenen� ��slo
	 */
	public static int getIntValue(String val) {
		int result = 0;
		int num = val.length();
		
		for (int i = 0; i < num; i++)
			if (val.charAt(i) == '1')
				result += Math.pow(2, num - i - 1);
		
		return result;
	}
}
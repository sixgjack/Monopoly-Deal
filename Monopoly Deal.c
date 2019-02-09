using System;
using System.Collections.Generic;


/*public delegate void PrintProp();
public delegate void PrintBank();
public delegate void PrintHand();
*/
public class Player
{
	public string name { get; private set; }
	public int CountTwoTurns;			//To count two turns under specified situations
	public List<Cards> Hand;
	public List<Money>[] Bank;
	public List<Properties>[] PropertiesDeck;
	public bool TurnEnd;         
	public bool TwoTurns;
	public bool GetTurn;
	public bool LastTurn;
	public bool Bankrupt;
	public bool Win { get; private set; }
	public string Name
	{
		get { return name; }
	}
	public Player(string name)
	{
		this.name = name;
		CountTwoTurns = 0;
		TurnEnd = false;
		TwoTurns = false;
		GetTurn = false;
		LastTurn = false;
		Hand = new List<Cards>();
		Bank = new List<Money>[6];
		for (int i = 0; i < 6; i++)
		{
			Bank[i] = new List<Money>();
		}
		PropertiesDeck = new List<Properties>[10];
		for (int i = 0; i < 10; i++)
		{
			PropertiesDeck[i] = new List<Properties>();
		}
		
	}
	/*-------------------------------------Model----------------------------------------------*/
	public void MoneyToBank(Money money)
	{
		if (money.value < 6)
			Bank[money.value - 1].Add(money);
		else if (money.value == 10)
			Bank[5].Add(money);
		Hand.Remove(money);
	}
	public void AddProperties(Properties prop)
	{
		if(prop.Polymorphism == false)
			PropertiesDeck[prop.colorIndex].Add(prop);
		else
			PropertiesDeck[prop.poly_colorIndex].Add(prop);
	}
	public void RemoveProperties(Properties prop)
	{
		PropertiesDeck[prop.colorIndex].Remove(prop);
	}
	public void Mortgage(Actions card)
	{
		card.IsMoney = true;
		Money money = new Money(card.Number, card.value);
		MoneyToBank(money);
	}

	public void Pay(Money money)
	{
		Bank[money.value - 1].Remove(money);
	}
	public void PayActions(Actions card)
	{
		Money money = new Money(card.Number, card.value);
		Bank[money.value - 1].Remove(money);
	}
	public void DealBreaker_Remove(int colorNum)
	{
		PropertiesDeck[colorNum].Clear();
	}
	public int PropNum(int colorIndex)
	{
		return (PropertiesDeck[colorIndex].Count);
	}
	public int MoneyNum(int value)
	{
		return (Bank[value].Count);
	}
	public bool PropIsFull(int colorIndex)
	{
		return (PropertiesDeck[colorIndex].Count == GameController.colormax[colorIndex]);
	}
	/*-------------------------View-----------------------*/
	public void PrintHand()
	{
		if (Hand.Count != 0)
		{
			foreach (Cards card in Hand)
				Console.WriteLine(card);
		}
		else
		{
			Console.WriteLine("Nothing!");
		}
	}
	public void PrintBank()
	{
		for (int i = 0; i < 6; i++)
		{
			if (Bank[i] != null)
			{
				foreach (Money money in Bank[i])
					Console.WriteLine(money);
			}
		}
	}
	public void PrintProperties()
	{

		for (int i = 0; i < 10; i++)
		{
			if (PropertiesDeck[i] != null)
			{
				foreach (Properties prop in PropertiesDeck[i])
					Console.WriteLine(prop);
			}
		}
	}
	public void PrintPropMoney()
	{

		for (int i = 0; i < 10; i++)
		{
			if (PropertiesDeck[i] != null)
			{
				foreach (Properties prop in PropertiesDeck[i])
					Console.WriteLine(prop+"  [Cost: $"+prop.value+"M]");
			}
		}
	}
	public void CheckWin()
	{
		int setCounter = 0;
		for(int i =0; i<10;i++)
		{
			if (PropertiesDeck[i]!= null)
			{
				foreach (Properties prop in PropertiesDeck[i])
				{
					if (prop.Polymorphism == true)
					{
						if (PropIsFull(prop.poly_colorIndex))
						{
							setCounter++;
							break;
						}
					}
					else {
						if (PropIsFull(prop.colorIndex))
						{
							setCounter++;
							break;
						}
					}
				}
			}
		}
		if (setCounter > 2)
			Win = true;
	}
	public override string ToString()
	{
		return name;
	}
}

public abstract class Cards : IComparable<Cards>
{
	public Cards(int num, int value)
	{
		Number = num;
		this.value = value;
	}
	public int Number { get; private set; }
	public int value { get; private set; }

	public int CompareTo(Cards cards)
	{
		if (value > cards.value) return 1;
		else if (value == cards.value) return 0;
		else return -1;
	}
}

public class Properties : Cards
{
	public int colorIndex { get; private set; }
	public bool Polymorphism;
	private string name;
	public string poly_WildCardname;
	public int poly_colorIndex;

	public Properties(int num, int value, int colorIndex, string name) : base(num, value)
	{
		this.colorIndex = colorIndex;
		this.name = name;
		Polymorphism = false;
	}
	
	public override string ToString()
	{
		if (Polymorphism == true)
			return "Property: [Wild Card as " + poly_WildCardname + "] in " + GameController.ColorIndex[poly_colorIndex] + "  [Card No." + Number + "]"+ " *Wild Card*"; 
		else
			return "Property: [" + name + "] in " + GameController.ColorIndex[colorIndex] + "  [Card No." + Number + "]";
	}
}

public abstract class Actions : Cards
{
	public Actions(int num, int value, string name) : base(num, value)
	{
		IsMoney = false;
		actionName = name;
	}
	// public abstract void Action();
	public string actionName;
	public bool IsMoney;
	public override string ToString()
	{
		return "Action: " + actionName + " [Cost: $"+value+"M]"+" [Card No." + Number + "]";
	}
}

public class Money : Cards
{
	public Money(int num, int value) : base(num, value)
	{
	}
	public override string ToString()
	{
		return "Money: $" + value + "M" + "  [Card No." + Number + "]";
	}
}

public class DealBreaker : Actions				//OK
{
	public DealBreaker(int num, int value) : base(num, value, "Deal Breaker")
	{
	}
	public void Action(Player me, Player target, int colorIndex)
	{
		
		if (target.PropIsFull(colorIndex))
		{
			foreach (Properties prop in target.PropertiesDeck[colorIndex])
				me.AddProperties(prop);
			target.DealBreaker_Remove(colorIndex);
			Console.WriteLine("HaHa! You have stolen a complete ["+GameController.ColorIndex[colorIndex]+"] set from "+target+"! Poor "+target+"!");
			Console.WriteLine();
		}
		else
		{
			Console.WriteLine("The targeted property set is not full yet! Card wasted!");
		}
	}
}

public class PassGo : Actions			//OK
{
	public PassGo(int num, int value) : base(num, value, "Pass Go")
	{
	}
	public void Action(Player me)
	{
		GameController.Instance.DrawCard(me);
		GameController.Instance.DrawCard(me);
	}
}
public class JustSayNo : Actions
{
	public JustSayNo(int num, int value) : base(num, value, "JustSayNo")
	{
	}
	public void Action(Actions cards)
	{
	}
}
public class ForcedDeal : Actions			//OK
{
	public ForcedDeal(int num, int value) : base(num, value, "Forced Deal")
	{
	}
	public void Action(Properties myProp, Properties targetProp, Player me, Player target)
	{
		if (!me.PropIsFull(myProp.colorIndex) && !target.PropIsFull(targetProp.colorIndex))
		{
			me.AddProperties(targetProp);
			target.AddProperties(myProp);
			me.RemoveProperties(myProp);
			target.RemoveProperties(targetProp);
			Console.WriteLine("Transcation succeed! Hope you may have a pleasure deal with "+ target+"!");
			Console.WriteLine();
		}
		else
		{
			Console.WriteLine("The property card is part of the full set! Card wasted!");
			Console.WriteLine();
		}
	}
}
public class SlyDeal : Actions		
{
	public SlyDeal(int num, int value) : base(num, value, "Sly Deal")
	{
	}
	public void Action(Properties targetProp, Player me, Player target)
	{
		if (!target.PropIsFull(targetProp.colorIndex))
		{
			me.AddProperties(targetProp);
			target.RemoveProperties(targetProp);
			Console.WriteLine("HaHa! You have stolen " + GameController.PropertyName[targetProp.Number] + " from " + target + "! Poor " + target + "!");
			Console.WriteLine();
		}
		else
		{
			Console.WriteLine("The property card is part of the full set! Card wasted!");
			Console.WriteLine();
		}
	}
}
public class DebtCollectors : Actions
{
	public DebtCollectors(int num, int value) : base(num, value, "Debt Collectors")
	{
	}
	public void Action(Player me, Player target)
	{
		bool HaveProp = false;
		bool HaveMoney = false;
		bool CanPay = false;
		for (int i = 0; i < 10; i++)
		{
			if (target.PropNum(i) > 0)
			{
				HaveProp = true;
				break;
			}
		}
		for (int i = 0; i < 6; i++)
		{
			if (target.MoneyNum(i) > 0)
			{
				HaveMoney = true;
				break;
			}
		}
		if (HaveMoney == true || HaveProp == true)
			CanPay = true;

		if (CanPay == true)
		{
			
			int payedMoney = GameController.Instance.PayMoney(me, target, 5);
			if (payedMoney == 5)
				Console.WriteLine("{0} payed you ${1}M", target, payedMoney);
			else if (payedMoney < 5)
				Console.WriteLine("{0} payed you ${1}M. {2} payed everything on the table.", target, payedMoney, target);
			else
				Console.WriteLine("{0} payed you ${1}M! No changes for {2}, sorry!", target, payedMoney, target);
		}
		else
		{
			if (CanPay == false)
				Console.WriteLine("There is nothing on {0}'s table, {1} can pay nothing to you.", target, target);
		}

		}
}
public class MyBirthday : Actions
{
	public MyBirthday(int num, int value) : base(num, value, "It's My Birthday")
	{
	}
	public void Action(Player me, Player target)
	{
		bool HaveProp = false;
		bool HaveMoney = false;
		bool CanPay = false;
		for (int i = 0; i < 10; i++)
		{
			if (target.PropNum(i) > 0)
			{
				HaveProp = true;
				break;
			}
		}
		for (int i = 0; i < 6; i++)
		{
			if (target.MoneyNum(i) > 0)
			{
				HaveMoney = true;
				break;
			}
		}
		if (HaveMoney == true || HaveProp == true)
			CanPay = true;

		if (CanPay == true)
		{
			int payedMoney = GameController.Instance.PayMoney(me, target, 2);
			if (payedMoney == 2)
				Console.WriteLine("{0} payed you ${1}M", target, payedMoney);
			else if (payedMoney < 2)
				Console.WriteLine("{0} payed you ${1}M. {2} payed everything on the table.", target, payedMoney, target);
			else
				Console.WriteLine("{0} payed you ${1}M! No changes for {2}, sorry!", target, payedMoney, target);
		}
		else
		{
			if (CanPay == false)
				Console.WriteLine("There is nothing on {0}'s table, {1} can pay nothing to you.", target, target);
		}
		Console.WriteLine();
	}
}
public class DoubleTheRent : Actions
{
	public DoubleTheRent(int num, int value) : base(num, value, "Double the Rent")
	{
	}
	public void Action(Player me, Player target, int colorIndex)
	{
		bool HaveProp = false;
		bool HaveMoney = false;
		bool CanPay = false;
		for (int i = 0; i < 10; i++)
		{
			if (target.PropNum(i) > 0)
			{
				HaveProp = true;
				break;
			}
		}
		for (int i = 0; i < 6; i++)
		{
			if (target.MoneyNum(i) > 0)
			{
				HaveMoney = true;
				break;
			}
		}
		if (HaveMoney == true || HaveProp == true)
			CanPay = true;

		if (me.PropNum(colorIndex) > 0 && CanPay == true)
		{
			int targetMoney = GameController.Rentprice[colorIndex][me.PropNum(colorIndex)] * 2;
			int payedMoney = GameController.Instance.PayMoney(me, target, targetMoney);
			if (targetMoney == payedMoney)
				Console.WriteLine("{0} payed you ${1}M", target, payedMoney);
			else if (targetMoney > payedMoney)
				Console.WriteLine("{0} payed you ${1}M. {2} payed everything on the table.", target, payedMoney, target);
			else
				Console.WriteLine("{0} payed you ${1}M! No changes for {2}, sorry!", target, payedMoney, target);
		}
		else
		{
			if (CanPay == false)
				Console.WriteLine("There is nothing on {0}'s table, {1} can pay nothing to you. Card Wasted!", target, target);
			else
				Console.WriteLine("You don't have any " + GameController.ColorIndex[colorIndex] + " Property in your Property Area. Card Wasted!");
		}
	}
}

public class Rent : Actions
{
	public int[] Rentcolor { get; private set; }
	public Rent(int num, int value, int colorIndex1, int colorIndex2) : base(num, value, "Rent")
	{
		Rentcolor = new int[2] { colorIndex1, colorIndex2 };
	}
	
	public void Action(Player me, Player target, int colorIndex)
	{
		bool HaveProp = false;
		bool HaveMoney = false;
		bool CanPay = false;
		for(int i = 0; i < 10; i++)
		{
			if(target.PropNum(i) > 0)
			{
				HaveProp = true;
				break;
			}
		}
		for (int i = 0; i < 6; i++)
		{
			if (target.MoneyNum(i) > 0)
			{
				HaveMoney = true;
				break;
			}
		}
		if (HaveMoney == true || HaveProp == true)
			CanPay = true;

		if (me.PropNum(colorIndex) > 0 && CanPay == true )
		{
			int targetMoney = GameController.Rentprice[colorIndex][me.PropNum(colorIndex)];
			int payedMoney = GameController.Instance.PayMoney(me, target, targetMoney);
			if(targetMoney == payedMoney)
			Console.WriteLine("{0} payed you ${1}M", target,payedMoney);
			else if(targetMoney > payedMoney)
				Console.WriteLine("{0} payed you ${1}M. {2} payed everything on the table.", target, payedMoney,target);
			else
				Console.WriteLine("{0} payed you ${1}M! No changes for {2}, sorry!", target, payedMoney,target);
		}
		else
		{
			if (CanPay == false)
				Console.WriteLine("There is nothing on {0}'s table, {1} can pay nothing to you.",target,target);
			else
			Console.WriteLine("You don't have any "+GameController.ColorIndex[colorIndex]+" Property in your Property Area."); 
		}
	}
	public override string ToString()
	{
		if (Rentcolor[0] == 10)
			return "Wild Rent [Card No." + Number + "]";
		else
			return GameController.ColorIndex[Rentcolor[0]] + " & " + GameController.ColorIndex[Rentcolor[1]] + " Rent [Card No." + Number + "]";
	}
}
public class GameController
{
	public static int[] colormax = { 2, 2, 2, 3, 3, 3, 3, 3, 3, 4 };
	public static Dictionary<int,string> ColorIndex { get; private set; }
	public static Dictionary<int, string> PropertyName { get; private set; }
	public static List<int>[] Rentprice { get; private set; }
	public static Dictionary<int, Cards> FullDeck { get; private set; }
	public bool IsText;
	public List<Cards> DiscardPile;
	public List<Cards> RemainingDeck;
	public List<int> DrawnCard;
	public List<Player> Players;
	public int DeckCounter;
	private int SteadyState1;
	private int SteadyState2;
	private bool reset;

	private GameController()
	{

		FullDeck = new Dictionary<int, Cards>();
		DiscardPile = new List<Cards>();
		RemainingDeck = new List<Cards>();
		DrawnCard = new List<int>();
		ColorIndex = new Dictionary<int, string>();
		PropertyName = new Dictionary<int, string>();
		Rentprice = new List<int>[10];
		Players = new List<Player>();
		SteadyState1 = 0;
		SteadyState2 = 0;
		DeckCounter = 0;
		reset = false;
		Init();
	}
	public void CreatePlayer(string name)
	{
		Players.Add(new Player(name));

	}
	private void PropnameInit()
	{
		PropertyName.Add(67, "blue1");
		PropertyName.Add(68, "blue2");
		PropertyName.Add(69, "brown1");
		PropertyName.Add(70, "brown2");
		PropertyName.Add(71, "utility1");
		PropertyName.Add(72, "utility2");
		PropertyName.Add(73, "green1");
		PropertyName.Add(74, "green2");
		PropertyName.Add(75, "green3");
		PropertyName.Add(76, "yellow1");
		PropertyName.Add(77, "yellow2");
		PropertyName.Add(78, "yellow3");
		PropertyName.Add(79, "red1");
		PropertyName.Add(80, "red2");
		PropertyName.Add(81, "red3");
		PropertyName.Add(82, "orange1");
		PropertyName.Add(83, "orange2");
		PropertyName.Add(84, "orange3");
		PropertyName.Add(85, "pink1");
		PropertyName.Add(86, "pink2");
		PropertyName.Add(87, "pink3");
		PropertyName.Add(88, "lightblue1");
		PropertyName.Add(89, "lightblue2");
		PropertyName.Add(90, "lightblue3");
		PropertyName.Add(91, "railroad1");
		PropertyName.Add(92, "railroad2");
		PropertyName.Add(93, "railroad3");
		PropertyName.Add(94, "railroad4");
	}
	private void CardInit()
	{
		
		for (int i = 0; i <= 5; i++) //1M
			FullDeck.Add(i, new Money(i, 1));
		for (int i = 6; i <= 10; i++) //2M
			FullDeck.Add(i, new Money(i, 2));
		for (int i = 11; i <= 13; i++) //3M
			FullDeck.Add(i, new Money(i, 3));
		for (int i = 14; i <= 16; i++) //4M
			FullDeck.Add(i, new Money(i, 4));
		for (int i = 17; i <= 18; i++) //5M
			FullDeck.Add(i, new Money(i, 5));
		for (int i = 19; i <= 19; i++) //10M
			FullDeck.Add(i, new Money(i, 10));
		for (int i = 20; i <= 22; i++) //Just say no
           	FullDeck.Add(i, new JustSayNo(i, 4));
		for (int i = 23; i <= 32; i++) //Pass go
			FullDeck.Add(i, new PassGo(i, 1));
		for (int i = 33; i <= 34; i++) //deal breaker
			FullDeck.Add(i, new DealBreaker(i, 5));
		for (int i = 35; i <= 37; i++) //Forced deal
			FullDeck.Add(i, new ForcedDeal(i, 3));
		for (int i = 38; i <= 40; i++) //Sly deal
			FullDeck.Add(i, new SlyDeal(i, 3));
		for (int i = 41; i <= 43; i++) //Deby collector
			FullDeck.Add(i, new DebtCollectors(i, 3));
		for (int i = 44; i <= 46; i++) //its my birthday
			FullDeck.Add(i, new MyBirthday(i, 2));
		for (int i = 47; i <= 48; i++) //double the rent
			FullDeck.Add(i, new DoubleTheRent(i, 1));
		/*
    	for (int i = 49; i <= 51; i++) //house
        	FullDeck.Add(new House(i, 3));
    	for (int i = 52; i <= 53; i++) //hotel
        	FullDeck.Add(new Hotel(i, 4));
    	*/
		for (int i = 54; i <= 55; i++) //blue green rent
			FullDeck.Add(i, new Rent(i, 1, 0, 3));
		for (int i = 56; i <= 57; i++) //red yellow rent
			FullDeck.Add(i, new Rent(i, 1, 5, 4));
		for (int i = 58; i <= 59; i++) //pink orange rent
			FullDeck.Add(i, new Rent(i, 1, 7, 6));
		for (int i = 60; i <= 61; i++) //lightblue broqn rent
			FullDeck.Add(i, new Rent(i, 1, 8, 1));
		for (int i = 62; i <= 63; i++) //railroad utility rent
			FullDeck.Add(i, new Rent(i, 1, 9, 2));
		for (int i = 64; i <= 66; i++) //wild rent
			FullDeck.Add(i, new Rent(i, 3, 10, 10));
		for (int i = 67; i <= 68; i++) //blue property
			FullDeck.Add(i, new Properties(i, 4, 0, PropertyName[i]));
		for (int i = 69; i <= 70; i++) //brown property
			FullDeck.Add(i, new Properties(i, 1, 1, PropertyName[i]));
		for (int i = 71; i <= 72; i++) //utility property
			FullDeck.Add(i, new Properties(i, 2, 2, PropertyName[i]));
		for (int i = 73; i <= 75; i++) //green property
			FullDeck.Add(i, new Properties(i, 4, 3, PropertyName[i]));
		for (int i = 76; i <= 78; i++) //yellow property
			FullDeck.Add(i, new Properties(i, 3, 4, PropertyName[i]));
		for (int i = 79; i <= 81; i++) //red property
			FullDeck.Add(i, new Properties(i, 3, 5, PropertyName[i]));
		for (int i = 82; i <= 84; i++) //orange property
			FullDeck.Add(i, new Properties(i, 2, 6, PropertyName[i]));
		for (int i = 85; i <= 87; i++) //pink property
			FullDeck.Add(i, new Properties(i, 2, 7, PropertyName[i]));
		for (int i = 88; i <= 90; i++) //lightblue property
			FullDeck.Add(i, new Properties(i, 1, 8, PropertyName[i]));
		for (int i = 91; i <= 94; i++) //railroad property
			FullDeck.Add(i, new Properties(i, 2, 9, PropertyName[i]));
		for (int i = 95; i <= 96; i++) //wild property
			FullDeck.Add(i, new Properties(i, 0, 10, "Wild Property"));
	}
	private void ColorindexInit()
	{
		ColorIndex.Add(0,"Blue");
		ColorIndex.Add(1,"Brown");
		ColorIndex.Add(2,"Utility");
		ColorIndex.Add(3,"Green");
		ColorIndex.Add(4,"Yellow");
		ColorIndex.Add(5,"Red");
		ColorIndex.Add(6,"Orange");
		ColorIndex.Add(7,"Pink");
		ColorIndex.Add(8,"Lightblue");
		ColorIndex.Add(9,"Railroad");
		ColorIndex.Add(10,"All");
	}
	private void RentpriceInit()
	{
		Rentprice[0] = new List<int> { 0, 3, 8 }; //blue
		Rentprice[1] = new List<int> { 0, 1, 2 }; //brown
		Rentprice[2] = new List<int> { 0, 1, 2 }; //utility
		Rentprice[3] = new List<int> { 0, 2, 4, 7 }; //green
		Rentprice[4] = new List<int> { 0, 2, 4, 6 }; //yellow
		Rentprice[5] = new List<int> { 0, 2, 3, 6 }; //red
		Rentprice[6] = new List<int> { 0, 1, 3, 5 }; //orange
		Rentprice[7] = new List<int> { 0, 1, 2, 4 }; //pink
		Rentprice[8] = new List<int> { 0, 1, 2, 3 }; //lightblue
		Rentprice[9] = new List<int> { 0, 1, 2, 3, 4 }; //railroad
	}
	static private GameController instance;
	static public GameController Instance
	{
		get
		{
			if (instance == null)
				instance = new GameController();
			return instance;
		}
	}
	private void Init()
	{
		PropnameInit();
		CardInit();
		ColorindexInit();
		RentpriceInit();

	}
	/*public void TextVersion(bool text)
	{
		IsText = text;
	}*/
	public void DrawCard(Player drawer)
	{
		Random r = new Random();
		int t;
		DeckCounter++;
		//Console.WriteLine(DeckCounter);		//Debug
		if (!CheckAllDrawn(drawer))
		{
			do
			{
				t = r.Next(0, 97);
			} while (t >= 49 && t < 54 || DrawnCard.Contains(t));
			Console.WriteLine("{0} draws a card.", drawer);

			DrawnCard.Add(t);

			drawer.Hand.Add(FullDeck[t]);
		}
		else
		{
			if(RemainingDeck[0] == null)
			{
				Console.WriteLine("You guys have drawn all cards without leaving one in the discard pile, the game comes to an end! A tie for everyone!");
				Console.WriteLine();
				Console.Write("Press [Enter] to leave..............");
				string press = Console.ReadLine();
			}
			do
				{
					t = r.Next(0, RemainingDeck.Count); 
				} while (t > RemainingDeck.Count || DrawnCard.Contains(t));
				DrawnCard.Add(t);
				Console.WriteLine("{0} draws a card.", drawer);
				drawer.Hand.Add(RemainingDeck[t]);
		}
	}
	public int PayMoney(Player me, Player target, int targetMoney)
	{
		int payedMoney = 0;
		Console.WriteLine("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		Console.WriteLine("|-------------------------{0}, time for you to pay!--------------------------------|",target);
		Console.WriteLine();
		Console.WriteLine("************************************Reminder****************************************");
		Console.WriteLine("You can pay with any combination of cards on the table in front of you that include money, property, action cards.");
		Console.WriteLine("You cannot pay with any cards in your hand. The cards must be laid on the table during your turn in order to be used for payment.");
		Console.WriteLine();
		Console.WriteLine("If you pay with property, that property goes into your opponents property section.");
		Console.WriteLine("Property cards can only be played into the property section.");
		Console.WriteLine();
		Console.WriteLine("Also,No change is given. If you do not have the exact amount, the opponent gets to keep the remaining!");
		Console.WriteLine();
		Console.WriteLine("*************************  "+target+", you have to pay [$"+targetMoney+"M]  **************************");
		Console.WriteLine();
		Console.WriteLine("This is your Bank and Properties:");
		Console.WriteLine("Property Area:");
		target.PrintPropMoney();
		Console.WriteLine();
		Console.WriteLine("Bank:");
		target.PrintBank();
		Console.WriteLine();
		payedMoney = CheckNumMoney(me,target,targetMoney);
		return payedMoney;
	}
	public void Turn(int seq, Player p)
	{
		if (p.TurnEnd == true)
		{
			//DeckCounter = FullDeck.Count-1;           //Debug
			return;
		}
		if (p.TwoTurns == true)
		{
			p.CountTwoTurns++;
			if (p.CountTwoTurns == 1)			//Reset
			{
				p.TwoTurns = false;
				p.CountTwoTurns = 0;
			}
			return;
		}
		if (p.GetTurn == true)
			return;

		Console.WriteLine();
		Console.WriteLine();
		Console.WriteLine("Cards in your hand:");
		p.PrintHand();
		Console.WriteLine();
		Console.WriteLine();
		Console.WriteLine("Property Area:");
		p.PrintProperties();
		Console.WriteLine();
		Console.WriteLine();
		Console.WriteLine("Bank:");
		p.PrintBank();
		Console.WriteLine();
		Console.WriteLine();
		Console.WriteLine("What to do now?");
		Textbase.CardSelect(p.Hand, seq,p);
	}

	public bool CheckAllDrawn(Player drawer)
	{
		Random r = new Random();
		if (DeckCounter == FullDeck.Count && reset == false)
		{
			Console.WriteLine();
			Console.WriteLine("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Attention!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			Console.WriteLine();
			Console.WriteLine();
			Console.Write("You have drawn all cards from the deck! Now shuffling the discard pile..........");
			Console.WriteLine();
			Console.WriteLine();
			Console.WriteLine("|------------------------------------------------------------------------------------|");
			SteadyState1 = DeckCounter;
			DeckCounter = 0;
			RemainingDeck = new List<Cards>(DiscardPile);
			DiscardPile.Clear();
			DrawnCard.Clear();
			reset = true;
			return true;
		}
		if (DeckCounter == RemainingDeck.Count && DeckCounter!=0 && reset == true)
		{
			Console.WriteLine();
			Console.WriteLine("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Attention!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			Console.WriteLine();
			Console.WriteLine();
			Console.Write("You have drawn all cards from the deck! Now shuffling the discard pile..........");
			Console.WriteLine();
			Console.WriteLine();
			Console.WriteLine("|------------------------------------------------------------------------------------|");
			RemainingDeck.Clear();
			RemainingDeck = new List<Cards>(DiscardPile);
			DiscardPile.Clear();
			DrawnCard.Clear();
			SteadyState1 = 0;
			SteadyState2 = DeckCounter;
			DeckCounter = 0;
			reset = false;
			return true;
		}
		if (DeckCounter == RemainingDeck.Count && DeckCounter != 0  && reset == false)
		{
			Console.WriteLine();
			Console.WriteLine("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Attention!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			Console.WriteLine();
			Console.WriteLine();
			Console.Write("You have drawn all cards from the deck! Now shuffling the discard pile..........");
			Console.WriteLine();
			Console.WriteLine();
			Console.WriteLine("|------------------------------------------------------------------------------------|");
			SteadyState1 = DeckCounter;
			SteadyState2 = 0;
			RemainingDeck.Clear();
			RemainingDeck = new List<Cards>(DiscardPile);
			DiscardPile.Clear();
			DrawnCard.Clear();
			DeckCounter = 0;
			reset = true;
			return true;
		}
		if (SteadyState1 > 0)
			return true;
		if (SteadyState2 > 0)
			return true;

		return false;
	}
	public void Polymorphism(Properties wildcard)
	{
		int num = 0;
		string input;
		wildcard.Polymorphism = true;
		Console.WriteLine();
		Console.WriteLine("Color Index:");
		for (int i = 0; i < 10; i++)
			Console.WriteLine(GameController.ColorIndex[i]+": "+i);
		Console.WriteLine();
		Console.WriteLine("Choose the color [Color Index] that you want the wildcard become:");
		Console.WriteLine();
		
		do
		{
			input = Console.ReadLine();
			bool isNum = true;

			if (input == "")
				isNum = false;
			foreach (Char c in input)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else
			{
				num = Convert.ToInt32(input);
				if (num < 0 || num > 10)
				{
					Console.WriteLine("Invalid [Color Index]! Please enter again!");
					num = 0;
					continue;
				}
				else break;
			}
		} while (true);

		wildcard.poly_colorIndex = num;
		wildcard.poly_WildCardname = ColorIndex[num];
	}
	public int YesorNo()
	{
		long num = -1;
		string input;
		do
		{
			input = Console.ReadLine();
			bool isNum = true;
			if (input == "")
				isNum = false;
			foreach (Char c in input)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else
			{
				num = Convert.ToInt64(input);
				if (num < 0||num > 1)
				{
					Console.WriteLine("Enter [1] for 'Yes' or [0] for 'No'. Try again!");
					num = -1;
					continue;
				}
				break;
			}
		} while (true);
		return (int)num;
	}
	public void CheckHand(Player p)
	{
		int card_no = 0;
		if(p.Hand.Count > 7)
		{
			Console.WriteLine();
			Console.WriteLine("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Attention!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			Console.WriteLine("Your hand is too full! You have to discard until there are 7 cards in your hand!");
			Console.WriteLine();
			Console.WriteLine("Cards in your hand:");
			p.PrintHand();
			Console.WriteLine();
			do
			{
				Console.WriteLine("Please enter the [Card No.] you want to discard:");
				card_no = CheckNumHand(p);
				p.Hand.Remove(FullDeck[card_no]);
				DiscardPile.Add(FullDeck[card_no]);
			} while (p.Hand.Count > 7);
			Console.WriteLine();
			
		}

	}
	public int CheckNum()
	{
		long num = 0;
		string input;
		do
		{
			input = Console.ReadLine();
			bool isNum = true;
			if (input == "")
				isNum = false;
			foreach (Char c in input)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else {
				num = Convert.ToInt64(input);
				if (num > 2147483647)
				{
					Console.WriteLine("Out of range! Try again!");
					num = 0;
					continue;
				}
				break;
			}
		} while (true);
		return (int)num;
	} 
	public int CheckNumInit()
	{
		long num = 0;
		string inputstring;
		do
		{
			Console.Write("Please enter number of players: ");
			inputstring = Console.ReadLine();
			bool isNum = true;
			if (inputstring == "")
				isNum = false;
			foreach (Char c in inputstring)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else
			{
				
				num = Convert.ToInt64(inputstring);
				if (num > 2147483647)
				{
					Console.WriteLine("Out of range! Try again!");
					num = 0;
					continue;
				}
				if (num > 4 || num < 2)
				{
					Console.WriteLine("Sorry! This is a 2-4 people game! Please enter again!");
					continue;
				}
				else break;
			}
		} while (true);
		return (int)num;
	}
	public int CheckNumHand(Player p)
	{
		int targetnum = 0;
		do
		{
			string input = Console.ReadLine();
			bool isNum = true;
			if (input == "-1")
				return -1;

			if (input == "")
				isNum = false;
			foreach (Char c in input)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else
			{
				targetnum = Convert.ToInt32(input);
				if (!p.Hand.Contains(FullDeck[targetnum]))
				{
					Console.WriteLine("Invalid [Card No.]!You don't have this card! Please enter again!");
					targetnum = 0;
					continue;
				}

				if (targetnum < 0 || targetnum > 96)
				{
					Console.WriteLine("Invalid card Number! Please enter again!");
					targetnum = 0;

					continue;
				}
				else break;
			}
		} while (true);
		return targetnum;
	}
	public int CheckNumCards(Player p)
	{
		int targetnum = 0;
		do
		{
			string input = Console.ReadLine();
			bool isNum = true;
			if (input == "-1")
				return -1;

			if (input == "")
				isNum = false;
			foreach (Char c in input)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else
			{	
				targetnum = Convert.ToInt32(input);

				if (targetnum < 0 || targetnum > 96)
				{
					Console.WriteLine("Invalid [Card No.]! Please enter again!");
					targetnum = 0;

					continue;
				}
				else if (!p.Hand.Contains(FullDeck[targetnum]))
				{
					Console.WriteLine("You don't have this card! Please enter again!");
					targetnum = 0;
					continue;
				} else break;
			}
		} while (true);
		return targetnum;
	}
	public int CheckNumRent(Player p)
	{
		int targetnum = 0;
		int checkcard = 0;
		bool noRent = false;
		for(int i = 54; i < 67; i++)
		{
			if((!p.Hand.Contains(FullDeck[i])))
			checkcard++;

			if (checkcard == 13)
				noRent = true;
		}
		if(noRent == true)
		{
			Console.WriteLine("You dont have any Rent Card in your hand! Card Wasted!");
			return -1;
		}
		do
		{
			string input = Console.ReadLine();
			bool isNum = true;
			if (input == "-1")
				return -1;

			if (input == "")
				isNum = false;
			foreach (Char c in input)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else
			{
				targetnum = Convert.ToInt32(input);

				if (targetnum < 54 || targetnum > 66)
				{
					Console.WriteLine("Invalid Rent [Card No.]! Please enter again!");
					targetnum = 0;
					continue;
				}else if (!p.Hand.Contains(FullDeck[targetnum]))
				{
						Console.WriteLine("You dont have that Rent Card in your hand! Enter again!");
						targetnum = 0;
						continue;
				}
				else break;
			}
		} while (true);
		
		return targetnum;
	}
	public bool CheckSayNo(Player me, Player target)
	{
		int decision = -1;
		for (int i = 20; i < 23; i++)
		{
			if (target.Hand.Contains(GameController.FullDeck[i]))
			{
				Console.WriteLine();
				Console.WriteLine("****************************************************************");
				Console.WriteLine("{0}, you have a [Just Say No] card! Are you gonna use it?", target);
				Console.Write("Enter [1] for 'Yes', [0] for 'No': ");
				decision = YesorNo();
				if (decision == 1)
				{
					DiscardPile.Add(GameController.FullDeck[i]);
					target.Hand.Remove(GameController.FullDeck[i]);
					if (Me_CheckSayNo(me,target) == true)
					{
						Console.WriteLine("{0}, you can't Say No since {1} Say No to your 'Say No'! HaHa!", target, me);
						Console.WriteLine();
						break;
					}
					else
					{
						Console.WriteLine("{0} says no to you {1}! HaHa!",target,me);
						Console.WriteLine();
						return true;
					}
				}
			}
		}
		
		return false;
	}
	public bool Me_CheckSayNo(Player me,Player target)
	{
		int decision = -1;
		for (int i = 20; i < 23; i++)
		{
			if (me.Hand.Contains(GameController.FullDeck[i]))
			{
				Console.WriteLine();
				Console.WriteLine("***************************************************************************");
				Console.WriteLine("{0}, you have a [Just Say No] card too! Are you gonna use it against that [Just Say No]?",me);
				Console.Write("Enter [1] for 'Yes', [0] for 'No': ");
				decision = YesorNo();
				if (decision == 1)
				{
					DiscardPile.Add(GameController.FullDeck[i]);
					me.Hand.Remove(GameController.FullDeck[i]);
					if (target_CheckSayNo(target) == true)
					{
						Console.WriteLine("Double Say No from {0}!", target);
						break;
					}
					else
						return true;
				}
			}
		}
		return false;
	}
	public bool target_CheckSayNo(Player target)
	{
		int decision = -1;
		for (int i = 20; i < 23; i++)
		{
			if (target.Hand.Contains(GameController.FullDeck[i]))
			{
				Console.WriteLine();
				Console.WriteLine("***************************************************************************");
				Console.WriteLine("Wow! {0}, you have one more [Just Say No] card! Are you gonna use it against that [Just Say No]?", target);
				Console.Write("Enter [1] for 'Yes', [0] for 'No': ");
				decision = YesorNo();
				if (decision == 1)
				{
					DiscardPile.Add(GameController.FullDeck[i]);
					target.Hand.Remove(GameController.FullDeck[i]);
					return true;
				}
			}
		}
		return false;
	}
	public int CheckNumMoney(Player me,Player target, int targetMoney)
	{
		int card_Num = 0;
		int payedMoney = 0;
		string input;
		bool HaveCard = false;
		bool HaveMoney = false;
		List<Cards> payedCards = new List<Cards>(); 
		do
		{
			Console.WriteLine("Please choose cards to pay (Enter the [Card No.]):");
			input = Console.ReadLine();
			bool isNum = true;
			
			if (input == "")
				isNum = false;
			foreach (Char c in input)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else
			{
				card_Num = Convert.ToInt32(input);
				
				if (card_Num < 0 && card_Num > 96)               
				{
					Console.WriteLine("Invalid [Card No.]! Please enter again!");
					card_Num = 0;
					continue;
				}
			}
			for (int i = 0; i < 6; i++)
			{
				if (target.Bank[i].Contains(FullDeck[card_Num] as Money))
				{
					HaveMoney = true;
					break;
				}
				else
					continue;
			}
			if (HaveMoney == false && card_Num < 20)
			{
				Console.WriteLine("The Money/[Action Card as Money] does not exists in your Bank! Try again!");
				card_Num = 0;
				continue;
			}
			for (int i = 0; i < 10; i++)
			{
				if (target.PropertiesDeck[i].Contains(FullDeck[card_Num] as Properties))
				{
					HaveCard = true;
					break;
				}
				else
					continue;
			}
			if (HaveCard == false && card_Num >66)
			{
				Console.WriteLine("The Propperty does not exists in your Property Area! Try again!");
				card_Num = 0;
				continue;
			}
			if (payedCards.Contains(FullDeck[card_Num]))
			{
				Console.WriteLine("You have already used this card! Try again!");
				card_Num = 0;
				HaveCard = false;
				HaveMoney = false;
				continue;
			}
				payedMoney += FullDeck[card_Num].value;
				payedCards.Add(FullDeck[card_Num]);
			
			if(card_Num>=67 && card_Num < 97)
			{
				target.RemoveProperties(FullDeck[card_Num] as Properties);
				me.AddProperties(FullDeck[card_Num] as Properties);
			}else
			if(card_Num>=0 && card_Num< 20)
			{
				target.Pay(FullDeck[card_Num] as Money);
				me.MoneyToBank(FullDeck[card_Num] as Money);
			}else if((FullDeck[card_Num] as Actions).IsMoney == true)
			{
				target.PayActions(FullDeck[card_Num] as Actions);
				me.Mortgage(FullDeck[card_Num] as Actions);
			}
			int remainingmoney = 0;
			if (targetMoney - payedMoney > 0)
				remainingmoney = targetMoney - payedMoney;

			Console.WriteLine("Payed: ${0}M. Still need${1}M",payedMoney,remainingmoney);
			card_Num = 0;
			HaveCard = false;
			HaveMoney = false;
		} while (payedMoney<targetMoney);

		return payedMoney;
	}
	public int CheckNumProp(Player target)
	{
		int targetnum = 0;
		bool IsEmpty = false;
		for (int i = 0; i < 10; i++)
		{
			if (target.PropertiesDeck[i].Count == 0)
				IsEmpty = true;
			else
			{
				IsEmpty = false;
				break;
			}
		}

		if (IsEmpty == true)
			return -1;

		bool HaveCard = false;
		do
		{
			string input = Console.ReadLine();
			bool isNum = true;

			if (input == "")
				isNum = false;
			foreach (Char c in input)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else
			{
				targetnum = Convert.ToInt32(input);
				if (targetnum < 67 || targetnum > 94)               //card No.95,96 is a wild card
				{
					Console.WriteLine("Invalid Property [Card No.]! Please enter again!");
					targetnum = 0;
					continue;
				}
			}
			for(int i = 0;i< 10; i++)
			{
				if (target.PropertiesDeck[i].Contains(FullDeck[targetnum] as Properties))
				{
					HaveCard = true;
					break;
				}
				else
					continue;
			}
			if (HaveCard == false)
			{
				Console.WriteLine("The Propertiy does not exists in the Property Area! Try again!");
				targetnum = 0;
				continue;
			}
			else break;
		} while (true);
		return targetnum;
	}
	public int CheckNumPlayers(int pNum, int seq)
	{
		int targetnum = 0;
		do
		{
			string input = Console.ReadLine();
			bool isNum = true;

			if (input == "")
				isNum = false;
			foreach (Char c in input)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else
			{
				targetnum = Convert.ToInt32(input);
				if (targetnum < 0 || targetnum > pNum || targetnum == seq)
				{
					Console.WriteLine("Invalid player number! Please enter again!");
					targetnum = 0;
					continue;
				}
				else break;
			}
		} while (true);
		return targetnum;
	}
	public int CheckRentColor(Rent rent)
	{
		int targetnum = 0;

		Console.WriteLine();
		
		Console.WriteLine();
		if (rent.Rentcolor[0] == 10)
		{
			Console.WriteLine("Color Index:");
			for (int i = 0; i < 10; i++)
				Console.WriteLine(GameController.ColorIndex[i] + ": " + i);
			Console.WriteLine("It's a [Wild Rent]! You may chosse whatever color you want!");
		}
		else
		{
			Console.WriteLine("The Rent Card has the following color:");
			Console.WriteLine(ColorIndex[rent.Rentcolor[0]] + " [Color Index]: " + rent.Rentcolor[0]);
			Console.WriteLine(ColorIndex[rent.Rentcolor[1]] + " [Color Index]: " + rent.Rentcolor[1]);
			Console.WriteLine();
		}
		Console.WriteLine("Please enter the [Color Index] that you want to choose (Enter a number):");
		do
		{
			string input = Console.ReadLine();
			bool isNum = true;
			if (input == "")
				isNum = false;
			foreach (Char c in input)
			{
				if (c < '0' || c > '9') isNum = false;
			}
			if (!isNum)
				Console.WriteLine("Please enter a number!");
			else
			{
				targetnum = Convert.ToInt32(input);
				
				if (targetnum < 0 || targetnum > 10)
				{
					Console.WriteLine("Invalid [Color Index]! Please enter again!");
					targetnum = 0;
					continue;
				}else if (targetnum != rent.Rentcolor[0] && targetnum != rent.Rentcolor[1] && rent.Rentcolor[0] != 10)
				{
					Console.WriteLine("Your Rent Card doesn't have this color! Please Enter the right one!");
					targetnum = 0;
					continue;
				}
				else break;
			}
		} while (true);
		return targetnum;
	}
}
public class Textbase
{
	private Textbase() { }
	static private Textbase instance;
	static public Textbase Instance
	{
		get
		{
			if (instance == null)
				instance = new Textbase();
			return instance;
		}
	}
	
	public static void CardSelect(List<Cards> cardlist, int seq,Player p)
	{
		int targetNum = 0;
		int TargetProp = 0;
		int myProp = 0;
		int targetRent = 0;
		int targetColor = 0;
		int YesorNo = 0;
		bool SayNo = false;

		Console.WriteLine("Please enter the card Number(Enter -1 if you don't want to do anything): ");
		int Card_No = GameController.Instance.CheckNumCards(p);
		if (Card_No == -1)
		{
			p.TurnEnd = true;
			return;
		}
		p.Hand.Remove(GameController.FullDeck[Card_No]);

		switch (Card_No)
		{
			case int n when (n >= 0 && n < 20):         //Money
				p.MoneyToBank(GameController.FullDeck[Card_No] as Money);
				break;
			case int n when (n >= 20 && n < 23):
				Console.WriteLine("You are using an Action Card. Turn it to money or not?");
				Console.Write("Enter [1] for 'Yes', [0] for 'No': ");
				Console.WriteLine();
				YesorNo = GameController.Instance.YesorNo();
				if (YesorNo == 1)
				{
					p.Mortgage(GameController.FullDeck[Card_No] as Actions);
					Console.WriteLine(GameController.FullDeck[Card_No] + " has turned into $" + GameController.FullDeck[Card_No].value + "M.");
					break;
				}
				Console.WriteLine("[Just Say No] can't be used individually. Try another card!");
				p.Hand.Add(GameController.FullDeck[Card_No]);
				p.GetTurn = true;
				break;
			case int n when (n>=23 && n<67):            //Action Cards
				Console.WriteLine("You are using an Action Card. Turn it to money or not?");
				Console.Write("Enter [1] for 'Yes', [0] for 'No': ");
				Console.WriteLine();
				YesorNo = GameController.Instance.YesorNo();
				if (YesorNo == 1)
				{
					p.Mortgage(GameController.FullDeck[Card_No] as Actions);
					Console.WriteLine(GameController.FullDeck[Card_No] + " has turned into $" + GameController.FullDeck[Card_No].value + "M.");
					break;
				}
				else
				{
				GameController.Instance.DiscardPile.Add(GameController.FullDeck[Card_No]);
					switch (n)
					{
						case int m when (m >= 23 && m < 33):        //PassGo
							for (int i = 1; i <= GameController.Instance.Players.Count; i++)
							{
								if (i - 1 == seq)
									continue;
								SayNo = GameController.Instance.CheckSayNo(p, GameController.Instance.Players[i - 1]);
							}
							if (SayNo == true)
								break;

							(GameController.FullDeck[Card_No] as PassGo).Action(p);
							break;

						case int m when (m >= 33 && m < 44 || m >= 64 && m < 67):
							Console.WriteLine();
							Console.WriteLine("Target Players:");
							for (int i = 1; i <= GameController.Instance.Players.Count; i++)
							{
								if (i - 1 == seq)
									continue;
								Console.WriteLine(i + ": {0}", GameController.Instance.Players[i - 1]);
							}
							Console.WriteLine();

							switch (m)                  //End of Action Cards that requires a target
							{
								case int j when (j > 32 && j < 35):     //Deal Breaker
									Console.WriteLine("You've used [Deal Breaker]! Choose your target(Enter the number): ");
									targetNum = GameController.Instance.CheckNumPlayers(GameController.Instance.Players.Count, seq + 1);
									Console.WriteLine("You have chosen " + GameController.Instance.Players[targetNum - 1] + "!");

									SayNo = GameController.Instance.CheckSayNo(p,GameController.Instance.Players[targetNum - 1]);
									if (SayNo == true)
										break;

									Console.WriteLine("He/She has the following Properties:"); 
									GameController.Instance.Players[targetNum - 1].PrintProperties();
									Console.WriteLine();
									Console.Write("Please enter the targeted property set ([Card No.] of the first card in the set):  ");
									TargetProp = GameController.Instance.CheckNumProp(GameController.Instance.Players[targetNum - 1]);
									if (TargetProp == -1)
									{
										Console.WriteLine();
										Console.WriteLine("The target has no property in the Property Area! Card wasted!!");
										break;
									}
									(GameController.FullDeck[Card_No] as DealBreaker).Action(p, GameController.Instance.Players[targetNum - 1], (GameController.FullDeck[TargetProp] as Properties).colorIndex);
									Console.WriteLine();
									break;
								case int j when (j >= 35 && j < 38):        //Forced Deal
									Console.WriteLine("You've used [Forced Deal]! Choose your target(Enter the number): ");
									targetNum = GameController.Instance.CheckNumPlayers(GameController.Instance.Players.Count, seq + 1);
									Console.WriteLine("You have chosen " + GameController.Instance.Players[targetNum - 1] + "!");

									SayNo = GameController.Instance.CheckSayNo(p, GameController.Instance.Players[targetNum - 1]);
									if (SayNo == true)
										break;

									Console.WriteLine("He/She has the following Properties:");
									GameController.Instance.Players[targetNum - 1].PrintProperties();
									Console.WriteLine();
									Console.WriteLine("Your Properties:");
									p.PrintProperties();
									Console.WriteLine();
									Console.Write("Please enter the targeted property [Card No.]:  ");
									TargetProp = GameController.Instance.CheckNumProp(GameController.Instance.Players[targetNum - 1]);
									if (TargetProp == -1)
									{
										Console.WriteLine();
										Console.WriteLine("The target has no property in the Property Area! Card wasted!!");
										break;
									}
									Console.Write("Please enter the your property [Card No.]:  ");
									myProp = GameController.Instance.CheckNumProp(p);
									if (myProp == -1)
									{
										Console.WriteLine();
										Console.WriteLine("You have no property in the Property Area to deal! Card wasted!!");
										break;
									}

									(GameController.FullDeck[Card_No] as ForcedDeal).Action(GameController.FullDeck[myProp] as Properties, GameController.FullDeck[TargetProp] as Properties, p, GameController.Instance.Players[targetNum - 1]);
									Console.WriteLine();
									break;
								case int j when (j >= 38 && j < 41):            //Sly Deal
									Console.WriteLine("You've used [Sly Deal]! Choose your target(Enter the number): ");
									targetNum = GameController.Instance.CheckNumPlayers(GameController.Instance.Players.Count, seq + 1);
									Console.WriteLine("You have chosen " + GameController.Instance.Players[targetNum - 1] + "!");

									SayNo = GameController.Instance.CheckSayNo(p, GameController.Instance.Players[targetNum - 1]);
									if (SayNo == true)
										break;

									Console.WriteLine("He/She has the following Properties:");
									Console.WriteLine();
									GameController.Instance.Players[targetNum - 1].PrintProperties();
									Console.WriteLine();
									Console.Write("Please enter the targeted property [Card No.]:  ");
									TargetProp = GameController.Instance.CheckNumProp(GameController.Instance.Players[targetNum - 1]);
									if (TargetProp == -1)
									{
										Console.WriteLine();
										Console.WriteLine("The target has no property in the Property Area! Card wasted!!");
										break;
									}
									(GameController.FullDeck[Card_No] as SlyDeal).Action(GameController.FullDeck[TargetProp] as Properties, p, GameController.Instance.Players[targetNum - 1]);
									Console.WriteLine();
									break;
								case int j when (j >= 41 && j < 44):            //Debt Collectors
									Console.WriteLine("You've used [Debt Collectors]! Choose your target(Enter the number): ");
									targetNum = GameController.Instance.CheckNumPlayers(GameController.Instance.Players.Count, seq + 1);
									Console.WriteLine("You have chosen " + GameController.Instance.Players[targetNum - 1] + "! HaHa!It's time for him/her to pay you $5M!");

									SayNo = GameController.Instance.CheckSayNo(p, GameController.Instance.Players[targetNum - 1]);
									if (SayNo == true)
										break;

									(GameController.FullDeck[Card_No] as DebtCollectors).Action(p, GameController.Instance.Players[targetNum - 1]);
									Console.WriteLine();
									break;
								case int j when (j >= 64 && j < 67):               //WildRent
									Console.WriteLine("You've used [Wild Rent]! Choose your target (Enter the number):");
									targetNum = GameController.Instance.CheckNumPlayers(GameController.Instance.Players.Count, seq + 1);
									Console.WriteLine("You have chosen " + GameController.Instance.Players[targetNum - 1] + "! HaHa!It's time for him/her to pay!");
									
									SayNo = GameController.Instance.CheckSayNo(p, GameController.Instance.Players[targetNum - 1]);
									if (SayNo == true)
										break;

									targetColor = GameController.Instance.CheckRentColor(GameController.FullDeck[Card_No] as Rent);

									(GameController.FullDeck[Card_No] as Rent).Action(p, GameController.Instance.Players[targetNum - 1], targetColor);
									break;
							}
							break;          //End of Action Cards that requires a target
						case int m when (m >= 44 && m < 47):        //MyBirthday
							Console.WriteLine("You've used [It's my Birthday]! Yay! All players have to pay you $2M!");
							for (int i = 1; i <= GameController.Instance.Players.Count; i++)
							{
								if (i == seq)
									continue;

								SayNo = GameController.Instance.CheckSayNo(p, GameController.Instance.Players[i - 1]);
								if (SayNo == true)
									break;

								(GameController.FullDeck[Card_No] as MyBirthday).Action(p, GameController.Instance.Players[i - 1]);
							}
							Console.WriteLine();
							break;
						case int m when (m >= 47 && m < 49):            //DoubleTheRent
							if (p.LastTurn == true)
							{
								Console.WriteLine("It's the last turn, you can't use this card since this card counts for two turns once used! Please use other card instead.");
								p.GetTurn = true;
								p.Hand.Add(GameController.FullDeck[Card_No]);
								GameController.Instance.DiscardPile.Remove(GameController.FullDeck[Card_No]);
								break;
							}
							Console.WriteLine("You've used [Double The Rent]! It's time to make them pay!");
							Console.WriteLine("Choose the Rent Card [Card No.] that you may want to double:");
							targetRent = GameController.Instance.CheckNumRent(p);
							if (targetRent == -1)
								break;
							else if (targetRent >= 64 && targetRent < 67)
							{
								targetColor = GameController.Instance.CheckRentColor(GameController.FullDeck[targetRent] as Rent);
								Console.WriteLine();
								Console.WriteLine("Target Playes:");
								for (int i = 1; i <= GameController.Instance.Players.Count; i++)
								{
									if (i - 1 == seq)
										continue;
									Console.WriteLine(i + ": {0}", GameController.Instance.Players[i - 1]);
								}
								Console.WriteLine("Choose your target (Enter the number):");
								targetNum = GameController.Instance.CheckNumPlayers(GameController.Instance.Players.Count, seq + 1);
								Console.WriteLine("You have chosen " + GameController.Instance.Players[targetNum - 1] + "! HaHa!It's time for him/her to pay!");

								SayNo = GameController.Instance.CheckSayNo(p, GameController.Instance.Players[targetNum - 1]);
								if (SayNo == true)
									break;


								(GameController.FullDeck[Card_No] as DoubleTheRent).Action(p, GameController.Instance.Players[targetNum - 1], targetColor);
								p.TwoTurns = true;
							}
							else
							{
								targetColor = GameController.Instance.CheckRentColor(GameController.FullDeck[targetRent] as Rent);
								Console.WriteLine("It's time to make them pay!");
								for (int i = 1; i <= GameController.Instance.Players.Count; i++)
								{
									if (i - 1 == seq)
										continue;

									SayNo = GameController.Instance.CheckSayNo(p, GameController.Instance.Players[i - 1]);
									if (SayNo == true)
										break;

									(GameController.FullDeck[Card_No] as DoubleTheRent).Action(p, GameController.Instance.Players[i - 1], targetColor);
								}
								p.TwoTurns = true;
							}
							break;
						case int m when (m >= 54 && m < 64):            //Rent
							Console.WriteLine("You've used [Rent]! It's time to make them pay!");
							targetColor = GameController.Instance.CheckRentColor(GameController.FullDeck[Card_No] as Rent);
							Console.WriteLine("You have chosen the " + GameController.ColorIndex[targetColor] + " Rent! It's time for them to pay!");
							for (int i = 1; i <= GameController.Instance.Players.Count; i++)
							{
								if (i - 1 == seq)
									continue;
								
								SayNo = GameController.Instance.CheckSayNo(p, GameController.Instance.Players[i - 1]);
								if (SayNo == true)
									break;

								(GameController.FullDeck[Card_No] as Rent).Action(p, GameController.Instance.Players[i - 1], targetColor);
							}
							break;
					}
					break;                 //End of Action Cards
				}
				case int n when (n >= 67 && n < 95):
				if (!p.PropIsFull((GameController.FullDeck[Card_No] as Properties).colorIndex))
				{
					p.AddProperties(GameController.FullDeck[Card_No] as Properties);
				}
				else
				{
					Console.WriteLine("The" + GameController.ColorIndex[(GameController.FullDeck[Card_No] as Properties).colorIndex] + " Property set is full! You can't add any more to it! Try again!");
					p.GetTurn = true;
					p.Hand.Add(GameController.FullDeck[Card_No]);
					GameController.Instance.DiscardPile.Remove(GameController.FullDeck[Card_No]);
				}
				break;
			case int n when (n >= 95 && n < 97):
				Console.WriteLine("It's a [Wild Property]! You can use it as part of any property set!");
				GameController.Instance.Polymorphism((GameController.FullDeck[Card_No] as Properties));
				p.AddProperties((GameController.FullDeck[Card_No] as Properties));
				break;
		}
	}
}
public class Program
{
	public static void Main()
	{
		Console.WriteLine("------------------------------Welcome to the Monopoly Deal!------------------------------------------");
		
		int numOfPlayers = GameController.Instance.CheckNumInit();
		string name;
		string[] Names = new string[numOfPlayers];
		List<int> Sequence = new List<int>();
		for (int i = 1; i <= numOfPlayers; i++)
		{
			do {
				Console.Write("Name of Player{0}: ", i);
				name = Console.ReadLine();
				if(name == "")
				Console.WriteLine("Names cannot leave blank! Enter again!"); 
			}
			while (name =="");
			Names[i - 1] = name;
			GameController.Instance.CreatePlayer(Names[i - 1]);
			Console.WriteLine();
		}
		Console.WriteLine("Now we have the following players:");
		for (int i = 1; i <= numOfPlayers; i++)
		{
			Console.WriteLine("Player {0}: {1}", i, GameController.Instance.Players[i - 1]);
		}
			Console.WriteLine();
		Console.WriteLine("Let's decide who to start first!");
		int seed = 0;
		for (int i = 0; i < numOfPlayers; i++)
		{
			Console.Write("{0} please enter a number optionally(From 0 to 2147483647):  ", GameController.Instance.Players[i]);
			seed += GameController.Instance.CheckNum();
			Console.WriteLine();
		}
		Random rnd = new Random(seed);
		Console.WriteLine("Thank you! Now let see who will start first");

		for (int i = 1; i <= numOfPlayers; i++)
		{
			int num = rnd.Next(0, numOfPlayers);
			while (Sequence.Contains(num))
				num = rnd.Next(0, numOfPlayers);
			Sequence.Add(num);
			Console.WriteLine(i + ": {0}", GameController.Instance.Players[Sequence[i - 1]]);
		}
		Console.WriteLine();
		Console.WriteLine("Everyone! Let's draw 5 cards first!");
		for(int i = 0;i<numOfPlayers;i++)
		{
			for(int j =0;j<5;j++)
			GameController.Instance.DrawCard(GameController.Instance.Players[Sequence[i]]);
		}
		Console.WriteLine();
		Console.WriteLine("Let's start the game!");
		Console.WriteLine();
		int counter = 0;
		while (true)
		{
			counter++;
			Console.WriteLine("---------------------------------{0}, it's your turn Now!------------------------------------------", GameController.Instance.Players[Sequence[counter - 1]]);
			Console.WriteLine();
			Console.WriteLine("**************************To use cards, please enter the [Card No.] instaed***********************************");

			GameController.Instance.DrawCard(GameController.Instance.Players[Sequence[counter - 1]]);
			GameController.Instance.DrawCard(GameController.Instance.Players[Sequence[counter - 1]]);

			if(GameController.Instance.RemainingDeck.Count == 0)
			Console.WriteLine("Cards Left: {0}", GameController.FullDeck.Count - GameController.Instance.DeckCounter);
			else
			Console.WriteLine("Cards Left: {0}", GameController.Instance.RemainingDeck.Count - GameController.Instance.DeckCounter);

			for (int j = 1; j < 4; j++)
			{
				Console.WriteLine("---------------------------------Turn {0}------------------------------------------", j);
				GameController.Instance.Turn(Sequence[counter - 1], GameController.Instance.Players[Sequence[counter - 1]]);
				GameController.Instance.Players[Sequence[counter - 1]].CheckWin();
				if (GameController.Instance.Players[Sequence[counter - 1]].Win == true)
				{
					Console.WriteLine("Congratuations!!!" + GameController.Instance.Players[Sequence[counter - 1]] + " has win the game!!!!!");
					break;
				}
				if(GameController.Instance.Players[Sequence[counter - 1]].GetTurn == true)
				{
					j --;
					GameController.Instance.Players[Sequence[counter - 1]].GetTurn = false;
				}
				if(j == 2)
					GameController.Instance.Players[Sequence[counter - 1]].LastTurn = true;
			}
			GameController.Instance.Players[Sequence[counter - 1]].TurnEnd = false;
			GameController.Instance.Players[Sequence[counter - 1]].LastTurn = false;
			Console.WriteLine();
			GameController.Instance.CheckHand(GameController.Instance.Players[Sequence[counter - 1]]);
			Console.WriteLine("**********************************Turn End****************************************************");
			Console.WriteLine();


			if (GameController.Instance.Players[Sequence[counter - 1]].Win == true)
				break;

			if (counter == numOfPlayers)
				counter = 0;
		}
		Console.WriteLine();
		Console.Write("Press [Enter] to leave.................");
		string press = Console.ReadLine();
	}
}



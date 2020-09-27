using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MyGeneticSharp.Chromosomes
{
    public class Chromosome<T> : IChromosome<T>
    {
        public double Fitness { get; set; }
        public int Length => Genes.Length;
        public T[] Genes { get; set; }
        public static Random random = new Random();

        public Chromosome(int length) => Genes = new T[length];

        protected Chromosome(Chromosome<T> source)
        {
            Genes = source.Genes.Clone() as T[];
            Fitness = source.Fitness;
        }

        public IChromosome<T> Clone() => new Chromosome<T>(this);
        
        public int CompareTo(object obj)
        {
            double fitness = (obj as Chromosome<T>).Fitness;
            double difference = fitness - Fitness;

            return (difference < 0.00001) ? 0 : (int)difference;
        }

        public void CrossOver(IChromosome<T> pair, Action<IChromosome<T>, IChromosome<T>> action) => action(this, pair);
        public void Evaluate(Action<IChromosome<T>> action) => action(this);
        public void Generate()
        {
            for(int i = 0; i < Length; ++i)
            {
                switch(Genes)
                {
                    case int[] _:
                        break;
                    case bool[] _:
                        Genes[i] = Convert.ToBoolean(random.Next(2)) as dynamic;                        
                        break;
                }
            }
        }

        public void Mutate(Action<IChromosome<T>> action) => action(this);

    }
}

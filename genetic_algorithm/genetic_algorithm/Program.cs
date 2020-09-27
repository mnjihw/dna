using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
/*using MyGeneticSharp;
using MyGeneticSharp.Chromosomes;*/
using Accord.Genetic;


namespace genetic_algorithm
{
    public class MyFitness : IFitnessFunction
    {
        private readonly int targetValue;
        public MyFitness(int value)
        {
            targetValue = value;
        }
        public double Evaluate(IChromosome chromosome)
        {
            BitArray target = new BitArray(new int[] { targetValue });
            BitArray bitArray = new BitArray(new int[] { (int)(chromosome as BinaryChromosome).Value });
            int count = 0;

            for(int i = 0; i < bitArray.Count; ++i)
            {
                if(target.Get(i) == bitArray.Get(i))
                    ++count;
            }
            return count;
        }
    }
    class Program
    {
        static async Task Main()
        {
            int targetValue = 2147421526;
            

            var chromosome = new BinaryChromosome(32);
            var population = new Population(40, chromosome, new MyFitness(targetValue), new EliteSelection())
            {
                AutoShuffling = true,
                MutationRate = 0.8
            };

            int count = 0;
            while (true)
            {
                population.RunEpoch();
                ++count;
                var fitness = population.BestChromosome.Fitness;
                
                if (fitness == 32)
                {
                    Console.WriteLine($"정답 : {(population.BestChromosome as BinaryChromosome).Value} {count}번만에 맞힘");
                    break;
                }
                
               
            }
            


            

           /* int size = 20;
            var chromosomes = new Chromosome<bool>[size];
            
            for(int i = 0; i < 10; ++i)
            {
                chromosomes[i] = new Chromosome<bool>(8);
                chromosomes[i].Generate();
                chromosomes[i].Evaluate(chromosome =>
                {
                    var genes = chromosome.Genes;
                    int count = 0;
                    foreach(var gene in genes)
                    {
                        if (gene == true)
                            ++count;
                    }
                    chromosome.Fitness = (double)count / genes.Length * 100;
                });
            }

            

            while(true)
            {
                var list = chromosomes.OrderByDescending(c => c.Fitness).Take(size / 3).Where(x => x.Fitness != 0.0);
                var lucky = chromosomes.OrderBy(x => Guid.NewGuid()).Take(size / 6).Where(x => x.Fitness != 0.0);

                var parents = Enumerable.Concat(list, lucky).ToList();

                while(parents.Count < size / 2)
                {
                    var chromosome = new Chromosome<bool>(8);
                    chromosome.Generate();
                    parents.Add(chromosome);

                }

                parents = parents.OrderBy(x => Guid.NewGuid()).ToList();

                for (int i = 0; i < parents.Count / 2; ++i)
                {
                    parents[i].CrossOver(parents[parents.Count - i - 1], (me, pair) =>
                    {
                        Chromosome<bool> child = new Chromosome<bool>(8);

                        int minLength = Math.Min(me.Length, pair.Length);
                        for(int j = 0; j < minLength; ++j)
                        {
                            if (Chromosome<bool>.random.Next(2) == 0)
                                child.Genes[j] = me.Genes[j];
                            else
                                child.Genes[j] = pair.Genes[j];
                        }
                    });
                }
            }*/
        }
    }
}

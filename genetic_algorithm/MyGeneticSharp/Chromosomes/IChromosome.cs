using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MyGeneticSharp.Chromosomes
{
    public interface IChromosome<T> : IComparable
    {
        double Fitness { get; set; }
        int Length { get; }
        T[] Genes { get; set; }
        void Generate();
        IChromosome<T> Clone();
        void Mutate(Action<IChromosome<T>> action);
        void CrossOver(IChromosome<T> pair, Action<IChromosome<T>, IChromosome<T>> action);
        void Evaluate(Action<IChromosome<T>> action);
    }
}

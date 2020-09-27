using System;
using System.Collections.Generic;
using System.Data.SqlClient;
using System.IO;
using System.Linq;
using Microsoft.ML;
using Microsoft.ML.Data;
using Microsoft.ML.Transforms.TimeSeries;

namespace ml.net
{
    class Program
    {
        static void Main(string[] args)
        {
            string rootDir = Path.GetFullPath(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "../../../"));
            string dataPath = Path.Combine(rootDir, "Data", "data.csv");
            string modelPath = Path.Combine(rootDir, "MLModel.zip");

            MLContext mlContext = new MLContext();

            var data = mlContext.Data.LoadFromTextFile<ModelInput>(dataPath, separatorChar: ',', hasHeader: true);
            
            var pipeline = mlContext.Transforms.CustomMapping<ModelInput, CustomMappingOutput>((input, output) =>
            {
                output.Mid = (input.High + input.Low) / 2;
            }, contractName: null);
            
            

            
            
            
            
/*
            mlContext.Data.FilterByCustomPredicate<ModelInput>(data, (a) =>
            {
                return true;
            });
            IDataView firstYearData = mlContext.Data.FilterRowsByColumn(data, "Date", upperBound: 1);
            IDataView secondYearData = mlContext.Data.FilterRowsByColumn(data, "Date", lowerBound: 1);

            var forecastingPipeline = mlContext.Forecasting.ForecastBySsa(
                outputColumnName: "ForecastedRentals",
                inputColumnName: "TotalRentals",
                windowSize: 7,
                seriesLength: 30,
                trainSize: 365,
                horizon: 7,
                confidenceLevel: 0.95f,
                confidenceLowerBoundColumn: "LowerBoundRentals",
                confidenceUpperBoundColumn: "UpperBoundRentals");

            SsaForecastingTransformer forecaster = forecastingPipeline.Fit(firstYearData);

            Evaluate(secondYearData, forecaster, mlContext);

            var forecastEngine = forecaster.CreateTimeSeriesEngine<ModelInput, ModelOutput>(mlContext);
            forecastEngine.CheckPoint(mlContext, modelPath);

            Forecast(secondYearData, 7, forecastEngine, mlContext);

            Console.ReadKey();*/
        }
/*
        static void Evaluate(IDataView testData, ITransformer model, MLContext mlContext)
        {
            // Make predictions
            IDataView predictions = model.Transform(testData);

            // Actual values
            IEnumerable<float> actual =
                mlContext.Data.CreateEnumerable<ModelInput>(testData, true)
                    .Select(observed => observed.TotalRentals);

            // Predicted values
            IEnumerable<float> forecast =
                mlContext.Data.CreateEnumerable<ModelOutput>(predictions, true)
                    .Select(prediction => prediction.ForecastedRentals[0]);

            // Calculate error (actual - forecast)
            var metrics = actual.Zip(forecast, (actualValue, forecastValue) => actualValue - forecastValue);

            // Get metric averages
            var MAE = metrics.Average(error => Math.Abs(error)); // Mean Absolute Error
            var RMSE = Math.Sqrt(metrics.Average(error => Math.Pow(error, 2))); // Root Mean Squared Error

            // Output metrics
            Console.WriteLine("Evaluation Metrics");
            Console.WriteLine("---------------------");
            Console.WriteLine($"Mean Absolute Error: {MAE:F3}");
            Console.WriteLine($"Root Mean Squared Error: {RMSE:F3}\n");
        }

        static void Forecast(IDataView testData, int horizon, TimeSeriesPredictionEngine<ModelInput, ModelOutput> forecaster, MLContext mlContext)
        {

            ModelOutput forecast = forecaster.Predict();

            IEnumerable<string> forecastOutput =
                mlContext.Data.CreateEnumerable<ModelInput>(testData, reuseRowObject: false)
                    .Take(horizon)
                    .Select((ModelInput rental, int index) =>
                    {
                        string rentalDate = rental.RentalDate.ToShortDateString();
                        float actualRentals = rental.TotalRentals;
                        float lowerEstimate = Math.Max(0, forecast.LowerBoundRentals[index]);
                        float estimate = forecast.ForecastedRentals[index];
                        float upperEstimate = forecast.UpperBoundRentals[index];
                        return $"Date: {rentalDate}\n" +
                        $"Actual Rentals: {actualRentals}\n" +
                        $"Lower Estimate: {lowerEstimate}\n" +
                        $"Forecast: {estimate}\n" +
                        $"Upper Estimate: {upperEstimate}\n";
                    });

            // Output predictions
            Console.WriteLine("Rental Forecast");
            Console.WriteLine("---------------------");
            foreach (var prediction in forecastOutput)
            {
                Console.WriteLine(prediction);
            }
        }*/
    }

    public class ModelInput
    {
        [LoadColumn(0)]
        public DateTime Date { get; set; }
        [LoadColumn(2)]
        public float High { get; set; }
        [LoadColumn(3)]
        public float Low { get; set; }        
    }

    public class ModelOutput
    {
        public DateTime Date { get; set; }
        public float Price { get; set; }
    }
    public class CustomMappingOutput
    {
        public float Mid { get; set; }
    }
}
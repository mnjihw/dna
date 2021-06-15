using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.ML;
using Microsoft.ML.Data;
using Microsoft.ML.Transforms.TimeSeries;


namespace ml.net
{
    public class ModelInput
    {
        public DateTime Date { get; set; }
        public float Close { get; set; }
    }

    public class ModelOutput
    {
        public float[] ForecastedClose { get; set; }
        public float[] LowerBoundClose{ get; set; }
        public float[] UpperBoundClose { get; set; }
    }
    class Program
    {
        static async Task Main(string[] args)
        {
            
            /* await using var stream = new FileStream("credentials.json", FileMode.Open, FileAccess.Read);
             var credential = GoogleWebAuthorizationBroker.AuthorizeAsync(GoogleClientSecrets.Load(stream).Secrets, new[] { "https://www.googleapis.com/auth/blogger" }, "user", CancellationToken.None, new FileDataStore("Auto.Blogger")).Result;
             using var service = new BloggerService(new BaseClientService.Initializer
             {
                 HttpClientInitializer = credential,
             });
             string blogId = "9126739708266783544";


             Post insertPost = new Post
             {

                 Title = "제목입니다",
                 Content = "<p>HTML콘텐츠를 여기에다가 넣으면 됩니다.</p>",
                 Published = "2100-12-31T00:00:00+09:00" // 등록 예약을 통한 글 비공개
             };
             var image = new ImagesData { Url = @"https://pds.joins.com/news/component/htmlphoto_mmdata/202012/30/d582edd8-4253-4a0c-aed2-c8a0fdf8d0f9.jpg.tn_350.jpg" };
             var list = new List<ImagesData>
             {
                 image
             };
             //insertPost.Images = list;
             insertPost.Content += @"<img src=""" + image.Url + @"""/>" ;

             Console.WriteLine($"'{insertPost.Title}' 글을 등록하겠습니다.");
             await service.Posts.Insert(insertPost, blogId).ExecuteAsync();
             Console.WriteLine($"'{insertPost.Title}' 글을 등록완료하였습니다.");


             return
            ;*/
            
            string rootDir = Path.GetFullPath(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "../../../"));
            string modelPath = Path.Combine(rootDir, "MLModel.zip");
            var csv = Path.Combine(rootDir, @"Data\data.csv");
            
            var columns = new[]
            {
                new TextLoader.Column("Date", DataKind.DateTime, 0),
                /*new TextLoader.Column("Open", DataKind.Single, 1),
                new TextLoader.Column("High", DataKind.Single, 2),
                new TextLoader.Column("Low", DataKind.Single, 3),*/
                new TextLoader.Column("Close", DataKind.Single, 4),
                /*new TextLoader.Column("AdjClose", DataKind.Single, 5),
                new TextLoader.Column("Volume", DataKind.UInt32, 6)*/
            };

            var mlContext = new MLContext();

            IDataView dataView = mlContext.Data.LoadFromTextFile(csv, columns, ',', hasHeader: true);


            
            var count = File.ReadAllLines(csv).Length - 1;
            var first = mlContext.Data.TakeRows(dataView, count / 2);
            var second = mlContext.Data.SkipRows(dataView, count / 2);


            var forecastingPipeline = mlContext.Forecasting.ForecastBySsa(
                outputColumnName: "ForecastedClose",
                inputColumnName: "Close",
                windowSize: 7,
                seriesLength: 30,
                trainSize: count / 2,
                horizon: 7,
                confidenceLevel: 0.95f,
                confidenceLowerBoundColumn: "LowerBoundClose",
                confidenceUpperBoundColumn: "UpperBoundClose");

            SsaForecastingTransformer forecaster = forecastingPipeline.Fit(first);

            Evaluate(second, forecaster, mlContext);

            var forecastEngine = forecaster.CreateTimeSeriesEngine<ModelInput, ModelOutput>(mlContext);
            forecastEngine.CheckPoint(mlContext, modelPath);

            Forecast(second, 7, forecastEngine, mlContext);

            Console.WriteLine("끝");
            
        }

        static void Evaluate(IDataView testData, ITransformer model, MLContext mlContext)
        {
            // Make predictions
            IDataView predictions = model.Transform(testData);

            // Actual values
            IEnumerable<float> actual =
                mlContext.Data.CreateEnumerable<ModelInput>(testData, true)
                    .Select(observed => observed.Close);

            // Predicted values
            IEnumerable<float> forecast =
                mlContext.Data.CreateEnumerable<ModelOutput>(predictions, true)
                    .Select(prediction => prediction.ForecastedClose[0]);

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
                    .Select((ModelInput input, int index) =>
                    {
                        string date = input.Date.ToShortDateString();
                        double actualClose= input.Close;
                        double lowerEstimate = Math.Max(0, forecast.LowerBoundClose[index]);
                        double estimate = forecast.ForecastedClose[index];
                        double upperEstimate = forecast.UpperBoundClose[index];
                        return $"Date: {date}\n" +
                        $"Actual Close: {actualClose}\n" +
                        $"Forecast: {estimate}\n" +
                        $"Lower Estimate: {lowerEstimate}\n" +
                        $"Upper Estimate: {upperEstimate}\n";
                    });

            // Output predictions
            Console.WriteLine("Close Forecast");
            Console.WriteLine("---------------------");
            foreach (var prediction in forecastOutput)
            {
                Console.WriteLine(prediction);
            }
        }
    }
}
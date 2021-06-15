using Instagram_archiver.Instagram;
using Instagram_archiver.Util;
using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Instagram_archiver.Db
{
    public class SqlExecutor : IDisposable
    {
        public MySqlCommand SqlCommand { get; set; }
        private bool DisposedValue { get; set; }

        public SqlExecutor(MySqlConnection sqlConnection)
        {
            SqlCommand = sqlConnection.CreateCommand();
            SqlCommand.Connection = sqlConnection;
            SqlCommand.CommandTimeout = 500;
        }


        private async Task BeginTransactionAsync()
        {
            SqlCommand.Transaction = await SqlCommand.Connection.BeginTransactionAsync();
        }

        private async Task CommitAsync()
        {
            await SqlCommand.Transaction.CommitAsync();
        }
        private async Task RollbackAsync()
        {
            await SqlCommand.Transaction.RollbackAsync();
        }

        private async Task DisposeTransactionAsync()
        {
            await SqlCommand.Transaction.DisposeAsync();
        }
        public async Task InsertAsync(Action action)
        {
            SqlCommand.Parameters.Clear();

            action();

            await BeginTransactionAsync();

            try
            {
                await SqlCommand.ExecuteNonQueryAsync();
                await CommitAsync();
                await DisposeTransactionAsync();
            }
            catch (Exception e)
            {
                try
                {
                    await Logger.SendTelegramMessageAsync(e.ToString());
                    await RollbackAsync();
                }
                catch (Exception e2)
                {
                    await Logger.SendTelegramMessageAsync(e2.ToString());
                }
            }

        }
        public async Task InsertImageAsync(InstagramImage image)
        {
            await InsertAsync(() =>
            {
                SqlCommand.Parameters.Add("@id", MySqlDbType.VarChar, 50).Value = image.Id;
                /*SqlCommand.Parameters.Add("@userId", MySqlDbType.VarChar, 50).Value = image.Post.Writer.Id;
                SqlCommand.Parameters.Add("@postId", MySqlDbType.VarChar, 50).Value = image.Post.Id;*/
                SqlCommand.Parameters.Add("@hash", MySqlDbType.VarChar, 70).Value = image.Hash;
                SqlCommand.CommandText = "INSERT INTO instagram_image (id, user_id, post_id, hash) VALUES (@id, @userId, @postId, @hash)";
            });
        }
        public async Task InsertUserAsync(InstagramUser user)
        {
            await InsertAsync(() =>
            {
                SqlCommand.Parameters.Add("@id", MySqlDbType.VarChar, 50).Value = user.Id;
                SqlCommand.Parameters.Add("@name", MySqlDbType.VarChar, 50).Value = user.Name;
                SqlCommand.CommandText = "INSERT INTO instagram_user (id, name) VALUES (@id, @name)";
            });
        }

        public async Task InsertPostAsync(InstagramPost post)
        {
            await InsertAsync(() =>
            {
                SqlCommand.Parameters.Add("@id", MySqlDbType.VarChar, 50).Value = post.Id;
                //SqlCommand.Parameters.Add("@userId", MySqlDbType.VarChar, 50).Value = post.Writer.Id;
                SqlCommand.Parameters.Add("@content", MySqlDbType.VarChar, 500).Value = post.Content;
                SqlCommand.CommandText = "INSERT INTO instagram_post (id, user_id, content, uploaded) VALUES (@id, @userId, @content, 0)";
            });
        }
        public async Task UpdatePostToUploadedAsync(string postId)
        {
            SqlCommand.Parameters.Clear();
            SqlCommand.Parameters.Add("@id", MySqlDbType.VarChar, 50).Value = postId;
            SqlCommand.CommandText = "UPDATE instagram_post SET uploaded = 1 WHERE id = @id";

            await BeginTransactionAsync();

            try
            {
                await SqlCommand.ExecuteNonQueryAsync();
                await CommitAsync();
                await DisposeTransactionAsync();
            }
            catch (Exception e)
            {
                try
                {
                    await Logger.SendTelegramMessageAsync(e.ToString());
                    await RollbackAsync();
                }
                catch (Exception e2)
                {
                    await Logger.SendTelegramMessageAsync(e2.ToString());
                }
            }
        }
        public async Task UpdateImageAsync(InstagramImage image)
        {
            SqlCommand.Parameters.Clear();
            SqlCommand.Parameters.Add("@id", MySqlDbType.VarChar, 50).Value = image.Id;
            /*SqlCommand.Parameters.Add("@userId", MySqlDbType.VarChar, 50).Value = image.Post.Writer.Id;
            SqlCommand.Parameters.Add("@postId", MySqlDbType.VarChar, 50).Value = image.Post.Id;*/
            SqlCommand.Parameters.Add("@hash", MySqlDbType.VarChar, 70).Value = image.Hash;
            SqlCommand.CommandText = "UPDATE instagram_image SET id = @id, user_id = @userId, post_id = @postId WHERE hash = @hash";

            await BeginTransactionAsync();

            try
            {
                await SqlCommand.ExecuteNonQueryAsync();
                await CommitAsync();
                await DisposeTransactionAsync();
            }
            catch (Exception e)
            {
                try
                {
                    await Logger.SendTelegramMessageAsync(e.ToString());
                    await RollbackAsync();
                }
                catch (Exception e2)
                {
                    await Logger.SendTelegramMessageAsync(e2.ToString());
                }
            }
        }
        public async Task<List<InstagramUser>> SelectImagesToUpload()
        {
            SqlCommand.Parameters.Clear();
            SqlCommand.CommandText = "SELECT hash, image.user_id, name, post_id, content FROM instagram_image AS image, instagram_post AS post, instagram_user AS USER WHERE uploaded = 0 AND image.user_id = post.user_id AND post.user_id = USER.id AND image.post_id = post.id";

            var users = new List<InstagramUser>();

            try
            {
                await using var reader = await SqlCommand.ExecuteReaderAsync();

                while (await reader.ReadAsync())
                {
                    var hash = reader.GetString("hash");
                    var userId = reader.GetString("user_id");
                    var userName = reader.GetString("name");
                    var postId = reader.GetString("post_id");
                    var content = reader.GetString("content");


                    var user = users.FirstOrDefault(user => user.Id == userId);

                    if(user != default)
                    {
                        var post = user.Posts.FirstOrDefault(post => post.Id == postId);

                        if(post != default)
                            post.Images.Add(new() { Hash = hash });
                        else
                        {
                            var images = new List<InstagramImage>() { new() { Hash = hash } };
                            user.Posts.Add(new() { Id = postId, Content = content, Images = images });
                        }
                    }
                    else
                    {
                        var images = new List<InstagramImage>() { new() { Hash = hash } };
                        var posts = new List<InstagramPost>() { new() { Id = postId, Content = content, Images = images } };

                        users.Add(new() { Id = userId, Name = userName, Posts = posts });
                    }

                }

            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
            }
            return users;

        }
        public async Task<(bool succeeded, string id, string userId, string postId)> SelectImageAsync(string hash)
        {
            SqlCommand.Parameters.Clear();
            SqlCommand.Parameters.Add("@hash", MySqlDbType.VarChar, 70).Value = hash;
            SqlCommand.CommandText = "SELECT * from instagram_image WHERE hash = @hash";

            try
            {
                await using var reader = await SqlCommand.ExecuteReaderAsync();

                if (await reader.ReadAsync())
                {
                    var id = reader.GetString("id");
                    var userId = reader.GetString("user_id");
                    var postId = reader.GetString("post_id");

                    return (true, id, userId, postId);
                }

            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
            }
            return default;

        }
        public async Task<int> DeleteImagesAsync(List<string> hashes)
        {
            var parameterNames = hashes.Select((hash, i) => $"@param{i}").ToArray();

            SqlCommand.Parameters.Clear();
            SqlCommand.CommandText = $"DELETE FROM instagram_image WHERE hash IN ({string.Join(',', parameterNames)})";

            for (int i = 0; i < parameterNames.Length; ++i)
                SqlCommand.Parameters.Add(parameterNames[i], MySqlDbType.VarChar).Value = hashes[i];

            await BeginTransactionAsync();

            int count = 0;

            try
            {
                count = await SqlCommand.ExecuteNonQueryAsync();
                await CommitAsync();
                await DisposeTransactionAsync();
            }
            catch (Exception e)
            {
                try
                {
                    await Logger.SendTelegramMessageAsync(e.ToString());
                    await RollbackAsync();
                }
                catch (Exception e2)
                {
                    await Logger.SendTelegramMessageAsync(e2.ToString());
                }

            }
            return count;

        }


        #region Dispose
        protected virtual void Dispose(bool disposing)
        {
            if (!DisposedValue)
            {
                if (disposing)
                {
                    // TODO: 관리형 상태(관리형 개체)를 삭제합니다.
                }
                SqlCommand.Dispose();

                // TODO: 비관리형 리소스(비관리형 개체)를 해제하고 종료자를 재정의합니다.
                // TODO: 큰 필드를 null로 설정합니다.
                DisposedValue = true;
            }
        }

        // // TODO: 비관리형 리소스를 해제하는 코드가 'Dispose(bool disposing)'에 포함된 경우에만 종료자를 재정의합니다.
        ~SqlExecutor()
        {
            // 이 코드를 변경하지 마세요. 'Dispose(bool disposing)' 메서드에 정리 코드를 입력합니다.
            Dispose(false);
        }

        public void Dispose()
        {
            // 이 코드를 변경하지 마세요. 'Dispose(bool disposing)' 메서드에 정리 코드를 입력합니다.
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}

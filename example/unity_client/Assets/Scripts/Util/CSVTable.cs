
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;

public class CSVTable : IEnumerable
{
	private Dictionary<string, int> columnNames = null;
	private List<List<string>> rows = null;

	public class Row
	{
		public Row(CSVTable table, List<string> row)
		{
			this.table = table;
			this.row = row;
		}

		public string GetValue(string columnName)
		{
			return row[table.GetIndex(columnName.ToLower())];
		}

		private List<string> row;
		private CSVTable table;
	}

	public void ReadStream(string stream)
	{
		columnNames = new Dictionary<string, int>();
		rows = new List<List<string>>();
		string[] data = stream.Split('\n');
		char[] trimChar = "\r\"".ToCharArray();
		string[] names = data[0].Trim(trimChar).Split(',');
		string[] types = data[1].Trim(trimChar).Split(',');

		for (int i = 0; i < names.Length; ++i)
		{
			if (true == String.IsNullOrEmpty(names[i]))
			{
				throw new System.Exception("error");
			}

			names[i] = names[i].Trim(trimChar).ToLower();
			columnNames.Add(names[i], i);
		}

		for (int i = 3; i < data.Length; ++i)
		{
			List<string> row = new List<string>(data[i].Trim(trimChar).Split(','));
			if (names.Length != row.Count)
			{
				continue;
			}
			for (int j = 0; j < row.Count; ++j)
			{
				row[j] = row[j].Trim(trimChar);
			}
			rows.Add(row);
		}
	}

	public void OpenFile(string fileName)
	{
		FileStream file = File.Open(fileName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

		byte[] buffer = new byte[file.Length];
		file.Read(buffer, 0, buffer.Length);
		file.Close();

		string text = System.Text.Encoding.Default.GetString(buffer, 0, buffer.Length);
		ReadStream(text);
	}

	public int GetIndex(string columnName)
	{
		if (false == columnNames.ContainsKey(columnName))
		{
			return -1;
		}

		return columnNames[columnName];
	}

	public Row GetRow(int index)
	{
		if (0 > index || rows.Count <= index)
		{
			throw new System.IndexOutOfRangeException();
		}

		Row row = new Row(this, rows[index]);
		return row;
	}

	public int GetRowCount()
	{
		return rows.Count;
	}

	IEnumerator IEnumerable.GetEnumerator()
	{
		for (int i = 0; i < rows.Count; i++)
		{
			yield return GetRow(i);
		}
	}
}




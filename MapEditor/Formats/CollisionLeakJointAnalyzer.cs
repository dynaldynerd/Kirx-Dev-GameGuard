using OpenTK.Mathematics;

namespace MapEditor.Formats;

public readonly record struct CollisionLeakJointRisk(
  int LineIndex,
  string Endpoint,
  int VertexIndex,
  int Degree,
  int NeighborLineIndex,
  float Score,
  float JointAngleDegrees,
  float LengthRatio,
  float LineLength,
  float NeighborLength,
  Vector3 VertexPosition);

public sealed class CollisionLeakJointReport
{
  public CollisionLeakJointReport(
    int scannedLineCount,
    float scoreThreshold,
    CollisionLeakJointRisk[] risks)
  {
    ScannedLineCount = scannedLineCount;
    ScoreThreshold = scoreThreshold;
    Risks = risks;
  }

  public int ScannedLineCount { get; }
  public float ScoreThreshold { get; }
  public CollisionLeakJointRisk[] Risks { get; }
}

public static class CollisionLeakJointAnalyzer
{
  private const float DefaultScoreThreshold = 2.0f;
  private const float MinLineLengthEpsilon = 0.0001f;
  private const int DefaultMaxResults = 64;

  public static CollisionLeakJointReport Analyze(
    LoadedMap map,
    float scoreThreshold = DefaultScoreThreshold,
    int maxResults = DefaultMaxResults)
  {
    ArgumentNullException.ThrowIfNull(map);

    Vector3[] vertices = map.CollisionVertices;
    CollisionLine[] lines = map.CollisionLines;
    if (vertices.Length == 0 || lines.Length == 0)
    {
      return new CollisionLeakJointReport(0, scoreThreshold, Array.Empty<CollisionLeakJointRisk>());
    }

    if (maxResults <= 0)
    {
      maxResults = DefaultMaxResults;
    }

    List<int>[] incidentLinesByVertex = BuildIncidentLineIndex(vertices.Length, lines);
    float[] lineLengths = new float[lines.Length];
    bool[] lineValid = new bool[lines.Length];

    int scannedLineCount = 0;
    for (int lineIndex = 1; lineIndex < lines.Length; ++lineIndex)
    {
      CollisionLine line = lines[lineIndex];
      if (!TryGetLineEndpoints(vertices, line, out Vector3 start, out Vector3 end))
      {
        continue;
      }

      float length = ComputeLengthXZ(start, end);
      if (!float.IsFinite(length) || length < MinLineLengthEpsilon)
      {
        continue;
      }

      lineLengths[lineIndex] = length;
      lineValid[lineIndex] = true;
      scannedLineCount++;
    }

    List<CollisionLeakJointRisk> risks = new();
    for (int lineIndex = 1; lineIndex < lines.Length; ++lineIndex)
    {
      if (!lineValid[lineIndex])
      {
        continue;
      }

      bool hasStartRisk = TryEvaluateEndpointRisk(
        map,
        lineIndex,
        atStartEndpoint: true,
        incidentLinesByVertex,
        lineLengths,
        lineValid,
        out CollisionLeakJointRisk startRisk);

      bool hasEndRisk = TryEvaluateEndpointRisk(
        map,
        lineIndex,
        atStartEndpoint: false,
        incidentLinesByVertex,
        lineLengths,
        lineValid,
        out CollisionLeakJointRisk endRisk);

      if (!hasStartRisk && !hasEndRisk)
      {
        continue;
      }

      CollisionLeakJointRisk chosenRisk = hasStartRisk && hasEndRisk
        ? (startRisk.Score >= endRisk.Score ? startRisk : endRisk)
        : (hasStartRisk ? startRisk : endRisk);

      if (chosenRisk.Score < scoreThreshold)
      {
        continue;
      }

      risks.Add(chosenRisk);
    }

    risks.Sort(static (left, right) =>
    {
      int scoreCompare = right.Score.CompareTo(left.Score);
      if (scoreCompare != 0)
      {
        return scoreCompare;
      }

      int lineCompare = left.LineIndex.CompareTo(right.LineIndex);
      if (lineCompare != 0)
      {
        return lineCompare;
      }

      return string.CompareOrdinal(left.Endpoint, right.Endpoint);
    });

    risks = DeduplicateMirroredJointRisks(risks);

    if (risks.Count > maxResults)
    {
      risks.RemoveRange(maxResults, risks.Count - maxResults);
    }

    return new CollisionLeakJointReport(scannedLineCount, scoreThreshold, risks.ToArray());
  }

  private static List<int>[] BuildIncidentLineIndex(int vertexCount, CollisionLine[] lines)
  {
    List<int>[] incident = new List<int>[vertexCount];
    for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
    {
      incident[vertexIndex] = new List<int>(4);
    }

    for (int lineIndex = 1; lineIndex < lines.Length; ++lineIndex)
    {
      CollisionLine line = lines[lineIndex];
      if (line.StartVertex >= vertexCount || line.EndVertex >= vertexCount)
      {
        continue;
      }

      incident[line.StartVertex].Add(lineIndex);
      incident[line.EndVertex].Add(lineIndex);
    }

    return incident;
  }

  private static bool TryEvaluateEndpointRisk(
    LoadedMap map,
    int lineIndex,
    bool atStartEndpoint,
    List<int>[] incidentLinesByVertex,
    float[] lineLengths,
    bool[] lineValid,
    out CollisionLeakJointRisk risk)
  {
    risk = default;
    CollisionLine line = map.CollisionLines[lineIndex];
    int endpointVertex = atStartEndpoint ? line.StartVertex : line.EndVertex;
    int oppositeVertex = atStartEndpoint ? line.EndVertex : line.StartVertex;
    if ((uint)endpointVertex >= (uint)map.CollisionVertices.Length || (uint)oppositeVertex >= (uint)map.CollisionVertices.Length)
    {
      return false;
    }

    Vector3 endpoint = map.CollisionVertices[endpointVertex];
    Vector3 opposite = map.CollisionVertices[oppositeVertex];
    Vector2 backDirection = new(opposite.X - endpoint.X, opposite.Z - endpoint.Z);
    if (backDirection.LengthSquared < MinLineLengthEpsilon)
    {
      return false;
    }

    List<int> incident = incidentLinesByVertex[endpointVertex];
    int degree = incident.Count;
    string endpointLabel = atStartEndpoint ? "start" : "end";

    if (degree <= 1)
    {
      // Open endpoint: high leak risk by definition.
      risk = new CollisionLeakJointRisk(
        lineIndex,
        endpointLabel,
        endpointVertex,
        degree,
        -1,
        Score: 4.0f,
        JointAngleDegrees: 180.0f,
        LengthRatio: 1.0f,
        LineLength: lineLengths[lineIndex],
        NeighborLength: 0.0f,
        VertexPosition: endpoint);
      return true;
    }

    bool found = false;
    float bestScore = float.MinValue;
    int bestNeighborLine = -1;
    float bestAngle = 180.0f;
    float bestLengthRatio = 1.0f;
    float bestNeighborLength = 0.0f;

    for (int i = 0; i < incident.Count; ++i)
    {
      int neighborLineIndex = incident[i];
      if (neighborLineIndex == lineIndex || !lineValid[neighborLineIndex])
      {
        continue;
      }

      CollisionLine neighborLine = map.CollisionLines[neighborLineIndex];
      int neighborOppositeVertex = neighborLine.StartVertex == endpointVertex
        ? neighborLine.EndVertex
        : neighborLine.StartVertex;
      if ((uint)neighborOppositeVertex >= (uint)map.CollisionVertices.Length)
      {
        continue;
      }

      Vector3 neighborOpposite = map.CollisionVertices[neighborOppositeVertex];
      Vector2 forwardDirection = new(neighborOpposite.X - endpoint.X, neighborOpposite.Z - endpoint.Z);
      if (forwardDirection.LengthSquared < MinLineLengthEpsilon)
      {
        continue;
      }

      float angle = ComputeAngleDegrees(backDirection, forwardDirection);
      float lengthRatio = ComputeLengthRatio(lineLengths[lineIndex], lineLengths[neighborLineIndex]);
      float score = ComputeRiskScore(
        angle,
        lengthRatio,
        degree);

      if (!found || score > bestScore)
      {
        found = true;
        bestScore = score;
        bestNeighborLine = neighborLineIndex;
        bestAngle = angle;
        bestLengthRatio = lengthRatio;
        bestNeighborLength = lineLengths[neighborLineIndex];
      }
    }

    if (!found)
    {
      return false;
    }

    risk = new CollisionLeakJointRisk(
      lineIndex,
      endpointLabel,
      endpointVertex,
      degree,
      bestNeighborLine,
      bestScore,
      bestAngle,
      bestLengthRatio,
      lineLengths[lineIndex],
      bestNeighborLength,
      endpoint);
    return true;
  }

  private static float ComputeRiskScore(float angleDegrees, float lengthRatio, int degree)
  {
    float score = 0.0f;

    // Non-straight joints are more vulnerable to slide-to-corner clipping.
    if (angleDegrees < 178.0f)
    {
      score += (178.0f - angleDegrees) / 18.0f;
    }

    if (angleDegrees < 165.0f)
    {
      score += 0.9f;
    }

    if (angleDegrees < 150.0f)
    {
      score += 0.9f;
    }

    // Large length discontinuity at a joint often indicates unstable resolve at the corner.
    if (lengthRatio >= 3.0f)
    {
      score += MathF.Min(3.0f, MathF.Log2(lengthRatio));
    }

    if (lengthRatio >= 8.0f)
    {
      score += 0.75f;
    }

    if (degree == 2)
    {
      score += 0.8f;
    }
    else if (degree >= 3)
    {
      score -= 0.4f;
    }

    return MathF.Max(0.0f, score);
  }

  private static float ComputeLengthRatio(float leftLength, float rightLength)
  {
    float minLength = MathF.Min(leftLength, rightLength);
    float maxLength = MathF.Max(leftLength, rightLength);
    if (!float.IsFinite(minLength) || minLength < MinLineLengthEpsilon)
    {
      return 1.0f;
    }

    return maxLength / minLength;
  }

  private static float ComputeAngleDegrees(Vector2 left, Vector2 right)
  {
    float leftLength = left.Length;
    float rightLength = right.Length;
    if (!float.IsFinite(leftLength) || !float.IsFinite(rightLength) || leftLength < MinLineLengthEpsilon || rightLength < MinLineLengthEpsilon)
    {
      return 180.0f;
    }

    float dot = Vector2.Dot(left, right) / (leftLength * rightLength);
    dot = MathF.Max(-1.0f, MathF.Min(1.0f, dot));
    return MathHelper.RadiansToDegrees(MathF.Acos(dot));
  }

  private static bool TryGetLineEndpoints(
    Vector3[] vertices,
    CollisionLine line,
    out Vector3 start,
    out Vector3 end)
  {
    start = default;
    end = default;
    if (line.StartVertex >= vertices.Length || line.EndVertex >= vertices.Length)
    {
      return false;
    }

    start = vertices[line.StartVertex];
    end = vertices[line.EndVertex];
    return IsFinite(start) && IsFinite(end);
  }

  private static float ComputeLengthXZ(Vector3 start, Vector3 end)
  {
    float dx = end.X - start.X;
    float dz = end.Z - start.Z;
    return MathF.Sqrt(dx * dx + dz * dz);
  }

  private static List<CollisionLeakJointRisk> DeduplicateMirroredJointRisks(List<CollisionLeakJointRisk> input)
  {
    if (input.Count <= 1)
    {
      return input;
    }

    List<CollisionLeakJointRisk> deduped = new(input.Count);
    HashSet<string> seenJointKeys = new(StringComparer.Ordinal);
    for (int i = 0; i < input.Count; ++i)
    {
      CollisionLeakJointRisk risk = input[i];
      string key;
      if (risk.NeighborLineIndex >= 0)
      {
        int left = Math.Min(risk.LineIndex, risk.NeighborLineIndex);
        int right = Math.Max(risk.LineIndex, risk.NeighborLineIndex);
        key = FormattableString.Invariant($"joint:{risk.VertexIndex}:{left}:{right}");
      }
      else
      {
        key = FormattableString.Invariant($"open:{risk.LineIndex}:{risk.Endpoint}:{risk.VertexIndex}");
      }

      if (!seenJointKeys.Add(key))
      {
        continue;
      }

      deduped.Add(risk);
    }

    return deduped;
  }

  private static bool IsFinite(Vector3 value)
  {
    return float.IsFinite(value.X) && float.IsFinite(value.Y) && float.IsFinite(value.Z);
  }
}

using OpenTK.Mathematics;

namespace MapEditor.Viewer;

internal sealed class FreeCamera
{
  public FreeCamera(Vector3 position, float yaw = -90f, float pitch = -20f)
  {
    Position = position;
    Yaw = yaw;
    Pitch = pitch;
  }

  public static FreeCamera CreateLookingAt(Vector3 position, Vector3 target)
  {
    Vector3 direction = target - position;
    if (direction.LengthSquared < 0.000001f)
    {
      return new FreeCamera(position);
    }

    direction = Vector3.Normalize(direction);
    float yaw = MathHelper.RadiansToDegrees(MathF.Atan2(direction.Z, direction.X));
    float pitch = MathHelper.RadiansToDegrees(MathF.Asin(direction.Y));
    return new FreeCamera(position, yaw, pitch);
  }

  public Vector3 Position { get; set; }
  public float Yaw { get; private set; }
  public float Pitch { get; private set; }
  public Vector3 Forward => GetForwardVector();
  public Vector3 Right => Vector3.Normalize(Vector3.Cross(Forward, Vector3.UnitY));

  public void AddRotation(float yawDelta, float pitchDelta)
  {
    Yaw += yawDelta;
    Pitch = MathHelper.Clamp(Pitch + pitchDelta, -89f, 89f);
  }

  public Matrix4 GetViewMatrix()
  {
    return Matrix4.LookAt(Position, Position + Forward, Vector3.UnitY);
  }

  private Vector3 GetForwardVector()
  {
    float yawRad = MathHelper.DegreesToRadians(Yaw);
    float pitchRad = MathHelper.DegreesToRadians(Pitch);

    Vector3 forward = new(
      MathF.Cos(yawRad) * MathF.Cos(pitchRad),
      MathF.Sin(pitchRad),
      MathF.Sin(yawRad) * MathF.Cos(pitchRad));

    return Vector3.Normalize(forward);
  }
}

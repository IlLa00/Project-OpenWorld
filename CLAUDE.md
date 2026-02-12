# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Project_OpenWorld** is an Unreal Engine 5.5 technical portfolio project for a client programmer position, focusing on open-world game development techniques. The project demonstrates advanced systems over a 2-month development period.

**Target Technical Features:**
1. Dynamic Weather System & Global Material Reactions
2. Large-scale Landscape with RVT (Runtime Virtual Texture) optimization
3. Post-Process & Atmosphere effects
4. StateTree-based NPC behavior that responds to weather conditions

**Language:** Korean-language project (documentation and comments may be in Korean)

## Project Structure

- **Engine Version:** Unreal Engine 5.5
- **Primary Module:** `Project_OpenWorld` (Runtime module)
- **Build System:** Unreal Build Tool (UBT)
- **Solution File:** `Project_OpenWorld.sln`
- **Project File:** `Project_OpenWorld.uproject`

### Key Directories

- `Source/Project_OpenWorld/` - C++ gameplay code and systems
- `Source/Project_OpenWorld.Target.cs` - Game target configuration
- `Source/Project_OpenWorldEditor.Target.cs` - Editor target configuration
- `Content/` - Unreal Engine assets (blueprints, materials, meshes, etc.)
- `Config/` - Project configuration files (.ini files)

## Building and Running

### Opening the Project

```bash
# Open in Unreal Editor (double-click or use command line)
start Project_OpenWorld.uproject
```

### Building from Command Line

**Generate Visual Studio project files:**
```bash
# Right-click Project_OpenWorld.uproject → "Generate Visual Studio project files"
# Or use UnrealBuildTool directly if needed
```

**Build the project:**
```bash
# Open Project_OpenWorld.sln in Visual Studio 2022
# Build → Build Solution (Ctrl+Shift+B)
# Or select Development Editor configuration and build
```

**Build configurations:**
- `DebugGame Editor` - Debug builds for editor
- `Development Editor` - Standard development builds for editor (recommended)
- `Shipping` - Final shipping builds

### Running

- **From Visual Studio:** Set `Project_OpenWorld` as startup project, press F5
- **From Unreal Editor:** Click Play button or Alt+P
- **Standalone:** Build and run from Launch → Project Launcher

## Code Architecture

### Module Dependencies

The main module (`Project_OpenWorld.Build.cs`) currently depends on:
- **Core** - Fundamental Unreal Engine types
- **CoreUObject** - UObject system
- **Engine** - Main engine functionality
- **InputCore** - Input handling
- **EnhancedInput** - New input system (UE5+)

### Rendering Configuration

The project is configured for high-end rendering (DefaultEngine.ini):
- **Lumen** enabled (r.DynamicGlobalIlluminationMethod=1, r.ReflectionMethod=1)
- **Ray Tracing** enabled (r.RayTracing=True)
- **Virtual Shadow Maps** enabled (r.Shadow.Virtual.Enable=1)
- **Mesh Distance Fields** enabled (r.GenerateMeshDistanceFields=True)
- **DirectX 12** with Shader Model 6 (SM6)
- Static lighting disabled (r.AllowStaticLighting=False)

### Important Configuration Notes

- The default map is set to `/Engine/Maps/Templates/OpenWorld`
- Hardware targeting is set to Desktop/Maximum performance
- The project uses enhanced input system (available in module dependencies)

## Development Workflow

### Adding New C++ Classes

1. Use Unreal Editor: Tools → New C++ Class
2. Or manually create .h/.cpp files in `Source/Project_OpenWorld/`
3. Update `Project_OpenWorld.Build.cs` if adding new module dependencies
4. Right-click .uproject → Generate Visual Studio project files
5. Build in Visual Studio

### Adding Module Dependencies

Edit `Source/Project_OpenWorld/Project_OpenWorld.Build.cs`:

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
    // Add new modules here
});
```

Common modules for planned features:
- **"Landscape"** - For large-scale terrain systems
- **"Foliage"** - For vegetation systems
- **"AIModule"** - For AI/StateTree implementations
- **"GameplayStateTree"** - For StateTree behavior
- **"Niagara"** - For advanced VFX (weather effects)
- **"RenderCore", "Renderer"** - For material/rendering systems

### Working with Blueprints and C++

- C++ classes automatically expose to Blueprint via `UCLASS()`, `UPROPERTY()`, `UFUNCTION()` macros
- Use `BlueprintReadWrite`, `BlueprintCallable` for Blueprint accessibility
- Hot reload is available but regenerating project files is more reliable

## Technical Implementation Notes

### For Dynamic Weather System
- Consider using Data Assets for weather state definitions
- Material Parameter Collections (MPC) for global material parameter control
- Niagara for particle-based weather effects (rain, snow)
- Post-process volumes for atmospheric scattering

### For Landscape & RVT
- Enable RVT in landscape settings
- Use Virtual Texturing for large terrains
- World Partition for level streaming (large worlds)
- Nanite for high-poly landscape meshes if needed

### For StateTree Integration
- StateTree is newer system (UE5.0+), ensure plugin is enabled if needed
- Consider GameplayStateTree module dependency
- States can read weather data via gameplay tags or global parameters

## Git Workflow

Current branch: `02.12` (date-based branch)
Main branch: `main`

The project uses Git for version control. Standard workflow applies.

## Notes

- Project documentation and comments may be in Korean
- Development timeline: 2 months
- This is a technical portfolio focusing on advanced rendering and gameplay systems
- Content directory is minimal at project initialization
